#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xtmrctr.h"
#include "xgpio.h"

#define LCD_DB4		0x01
#define LCD_DB5		0x02
#define LCD_DB6		0x04
#define LCD_DB7		0x08
#define LCD_RW		0x10
#define LCD_RS		0x20
#define LCD_E		0x40

XGpio GpioOutput;
XTmrCtr DelayTimer;

void delay_us(u32 time);
void delay_ms(u32 time);
void gpio_write(u32 c);
u32 gpio_read(void);
void lcd_clk(void);
void lcd_set_rs(void);
void lcd_reset_rs(void);
void lcd_set_rw(void);
void lcd_reset_rw(void);
void lcd_write(u32 c);
void lcd_clear(void);
void lcd_puts(const char* s);
void lcd_putch(u32 c);
void lcd_goto(u32 line, u32 pos);
void lcd_init(void);

int main(void)
{
	int Status;

	// Initialize the Timer
	Status = XTmrCtr_Initialize(&DelayTimer, XPAR_TMRCTR_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Timer failed to initialize\r\n");
		return Status;
	}
	XTmrCtr_SetOptions(&DelayTimer, 1, XTC_DOWN_COUNT_OPTION);

	// Initialize the GPIO driver for the LCD
	Status = XGpio_Initialize(&GpioOutput,
							XPAR_GPIO_LCD_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("GPIO failed to initialize\r\n");
		return Status;
	}
	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&GpioOutput, 1, 0x00);	

	xil_printf("16x2 LCD Driver by Spartan-6 Resource\r\n");

	// Initialize the LCD
	lcd_init();

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Example write to the LCD
	lcd_puts("http://www.noticekorea");
	lcd_goto(1, 2);
	lcd_puts("ekorea.com");

	while (1);

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

// Delay function (microseconds)
void delay_us(u32 time)
{
	XTmrCtr_SetResetValue(&DelayTimer, 1, time * (XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ/1000000));
	XTmrCtr_Start(&DelayTimer, 1);
	while(!(XTmrCtr_IsExpired(&DelayTimer, 1)));
	XTmrCtr_Stop(&DelayTimer, 1);
}

// Delay function (milliseconds)
void delay_ms(u32 time)
{
	XTmrCtr_SetResetValue(&DelayTimer, 1, time * (XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ/1000));
	XTmrCtr_Start(&DelayTimer, 1);
	while(!(XTmrCtr_IsExpired(&DelayTimer, 1)));
	XTmrCtr_Stop(&DelayTimer, 1);
}

// Write to GPIO outputs
void gpio_write(u32 c)
{
	// Write to the GP IOs
	XGpio_DiscreteWrite(&GpioOutput, 1, c & 0xFF);
}

// Read the GPIO outputs
u32 gpio_read()
{
	// Read from the GP IOs
	return(XGpio_DiscreteRead(&GpioOutput, 1));
}

// Clock the LCD (toggles E)
void lcd_clk()
{
	u32 c;

	// Get existing outputs
	c = gpio_read();
	delay_us(1);
	// Assert clock signal
	gpio_write(c | LCD_E);
	delay_us(1);
	// Deassert the clock signal
	gpio_write(c & (~LCD_E));
	delay_us(1);
}

// Assert the RS signal
void lcd_set_rs()
{
	u32 c;

	// Get existing outputs
	c = gpio_read();
	// Assert RS
	gpio_write(c | LCD_RS);
	delay_us(1);
}

// Deassert the RS signal
void lcd_reset_rs()
{
	u32 c;

	// Get existing outputs
	c = gpio_read();
	// Deassert RS
	gpio_write(c & (~LCD_RS));
	delay_us(1);
}

// Assert the RW signal
void lcd_set_rw()
{
	u32 c;

	// Get exisiting outputs
	c = gpio_read();
	// Assert RW
	gpio_write(c | LCD_RW);
	delay_us(1);
}

// Deassert the RW signal
void lcd_reset_rw()
{
	u32 c;

	// Get existing outputs
	c = gpio_read();
	// Deassert RW
	gpio_write(c & (~LCD_RW));
	delay_us(1);
}

// Write a byte to LCD (4 bit mode)
void lcd_write(u32 c)
{
	u32 temp;

	// Get existing outputs
	temp = gpio_read();
	temp = temp & 0xF0;
	// Set the high nibble
	temp = temp | ((c >> 4) & 0x0F);
	gpio_write(temp);
	// Clock
	lcd_clk();
	// Delay for "Write data into internal RAM 43us"
	delay_us(100);
	// Set the low nibble
	temp = temp & 0xF0;
	temp = temp | (c & 0x0F);
	gpio_write(temp);
	// Clock
	lcd_clk();
	// Delay for "Write data into internal RAM 43us"
	delay_us(100);
}

// Clear LCD
void lcd_clear()
{
	lcd_reset_rs();
	// Clear LCD
	lcd_write(0x01);
	// Delay for "Clear display 1.53ms"
	delay_ms(2);
}

// Write a string to the LCD
void lcd_puts(const char* s)
{
	lcd_set_rs();
	while (*s)
		lcd_write(*s++);
}

// Write character to the LCD
void lcd_putch(u32 c)
{
	lcd_set_rs();
	lcd_write(c);
}

// Change cursor position
// (line = 0 or 1, pos = 0 to 15)
void lcd_goto(u32 line, u32 pos)
{
	lcd_reset_rs();
	pos = pos & 0x3F;
	if (line == 0)
		lcd_write(0x80 | pos);
	else
		lcd_write(0xC0 | pos);
}

// Initialize the LCD
void lcd_init()
{
	u32 temp;

	// Write mode (always)
	lcd_reset_rw();
	// Write control bytes
	lcd_reset_rs();

	// Delay 15ms
	delay_ms(15);

	// Initialize
	temp = gpio_read();
	temp = temp | LCD_DB5;
	gpio_write(temp);
	lcd_clk();
	lcd_clk();
	lcd_clk();

	// Delay 15ms
	delay_ms(15);

	// Function Set: 4 bit mode, 1/16 duty, 5x8 font, 2 lines
	lcd_write(0x28);
	// Display ON/OFF Control: ON
	lcd_write(0x0C);
	// Entry Mode Set: Increment (cursor moves forward)
	lcd_write(0x06);

	// Clear the display
	lcd_clear();
}

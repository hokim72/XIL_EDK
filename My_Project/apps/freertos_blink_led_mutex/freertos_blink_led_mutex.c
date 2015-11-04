#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "xil_printf.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xgpio.h"

#define GPIO_DEVICE_ID	XPAR_LEDS_4BITS_DEVICE_ID

#define LED_CHANNEL	1

#define BLINK_PERIOD 1000

#define mainLED_ON_TASK_PRIORITY	(tskIDLE_PRIORITY + 1)
#define mainLED_OFF_TASK_PRIORITY	(tskIDLE_PRIORITY + 1)

static void prvLed_ON(void* pvParameters);
static void prvLed_OFF(void* pvParameters);
int prvSetGpioHardware(void);
void prvLed_Toggle(int Mode);

XGpio GpioOutput;

xSemaphoreHandle xMutex_Led = NULL;

// This example illustrates synchronization of two tasks using mutex.
// The example shows the usage of Mutex and different non-priority based
// scheduling algorithms.
// Two theads of same priority are created to toggle the 4-bit LED. One thread
// puts the LED in ON state and the other in OFF state. The toggle LED function
// is protected by a Mutex.
// Each thread calls toggle LED function with appropriate argument (ON or OFF).
// Toggle LED function takes the mutex and toggles the LED. To ensure 50%
// duty cycle, the thread that takes the mutex sleeps keeping the LED in the
// relevant state. Because of the scheduling algorithm, the control transfers
// to other thread which agin calls the toggle LED function with appropriate 
// argument.
// This thread tries to take the Mutex and blocks as it is held by first thread.
// After the first thread comes out of sleep, it release the mutex and then
// yields so that the second thread can toggle the LED to then relevant state.
// Second thread then puts the LED in appropriate state and sleeps. This
// control flow goes on.

int main(void)
{
    microblaze_disable_interrupts();

    #if defined( XPAR_MICROBLAZE_USE_ICACHE ) && ( XPAR_MICROBLAZE_USE_ICACHE != 0 )
    {
        Xil_ICacheInvalidate();
        Xil_ICacheEnable();
    }
    #endif

    #if defined( XPAR_MICROBLAZE_USE_DCACHE ) && ( XPAR_MICROBLAZE_USE_DCACHE != 0 )
    {
        Xil_DCacheInvalidate();
        Xil_DCacheEnable();
    }
    #endif

	xMutex_Led = xSemaphoreCreateMutex();

	configASSERT(xMutex_Led);

	// Setup the GPIO Hardware.
	prvSetGpioHardware();

	// Start the two tasks.
	xTaskCreate(prvLed_ON, "LED_ON",
				configMINIMAL_STACK_SIZE, NULL,
				mainLED_ON_TASK_PRIORITY, NULL);

	xTaskCreate(prvLed_OFF, "LED_OFF",
				configMINIMAL_STACK_SIZE, NULL,
				mainLED_OFF_TASK_PRIORITY, NULL);

	// Start the tasks and timer running.
	vTaskStartScheduler();

	// If all is well, the scheduler will now be running, and the following line
	// will never be reached. If the following line does execute, then there was
	// insufficient FreeRTOS heap memory available for the idle and/or timer
	// tasks to be created. See the memory management section on the FreeRTOS
	// web site for more details.
	for (;;);
}

static void prvLed_ON(void* pvParameters)
{
	for (;;)
	{
		prvLed_Toggle(0x1);
		taskYIELD();
	}
}

static void prvLed_OFF(void* pvParameters)
{
	for (;;)
	{
		prvLed_Toggle(0x0);
		taskYIELD();
	}
}

void prvLed_Toggle(int Mode)
{
	portTickType xNextWakeTime;
	static u32 Data = 1;

	xSemaphoreTake(xMutex_Led, portMAX_DELAY);

	// Set the GPIO Output for each 4 LEDs to the value of Mode
	if (Mode == 0) Data<<=1;
	if (Data == 16) Data = 1;
	XGpio_DiscreteWrite(&GpioOutput, LED_CHANNEL, Mode?Data:0);

	if (Mode == 0x1)
		xil_printf("LED ON\r\n");
	else
		xil_printf("LED OFF\r\n");

	// Initialize xNextWakeTime - this only needs to be done once.
	xNextWakeTime = xTaskGetTickCount();

	// Place this task in the blocked state until it is time to run again.
	// The block time is specified in ticks, the constant used converts ticks
	// to ms. While in the Blocked state this task will not consume any CPU
	// time.
	vTaskDelayUntil(&xNextWakeTime, BLINK_PERIOD);

	xSemaphoreGive(xMutex_Led);
}

int prvSetGpioHardware(void)
{
	int Status;
	// Initialize the GPIO driver so that it's ready to use,
	// specify the device ID that is generated in xparameters.h
	Status = XGpio_Initialize(&GpioOutput, GPIO_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&GpioOutput, LED_CHANNEL, 0x0);

	// Set the GPIO outputs to low
	XGpio_DiscreteWrite(&GpioOutput, LED_CHANNEL, 0x0);
	return Status;
}

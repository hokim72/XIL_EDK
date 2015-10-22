#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xgpio.h"
#include "xil_printf.h"

#define LED_CHANNEL 1
#define SWITCH_CHANNEL 1
#define LED_DELAY 1000000


int main(void)
{
	XGpio GpioOutput; // The driver instance for GPIO Device configured as O/P
	XGpio GpioInput; // The driver instance for GPIO Device configured as I/P
	u32 Data;
	volatile int Delay;
	u32 LedBit;
	int Status;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the GPIO driver so that it's ready to use,
	// specify the device ID that is generated in xparameters.h
	Status = XGpio_Initialize(&GpioOutput, XPAR_LEDS_4BITS_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&GpioOutput, LED_CHANNEL, 0x0);

	// Set the GPIO outputs to low
	XGpio_DiscreteWrite(&GpioOutput, LED_CHANNEL, 0x0);

	for (LedBit = 0x0; LedBit<4; LedBit++) {
		// Set the GPIO Output to High
		XGpio_DiscreteWrite(&GpioOutput, LED_CHANNEL, 1 << LedBit);

		// Wait a small amount of time so the LED is visible
		for (Delay=0; Delay<LED_DELAY; Delay++);

		// Clear the GPIO Ouput
		XGpio_DiscreteClear(&GpioOutput, LED_CHANNEL, 1 << LedBit);
		
		// Wait a small amount of time so the LED is visible
		for (Delay=0; Delay<LED_DELAY; Delay++);
	}

	// Initialize the GPIO driver so that it's ready to use,
	// specify the device ID that is generated in xparameters.h
	Status = XGpio_Initialize(&GpioInput, XPAR_DIP_SWITCHES_4BITS_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be inputs
	XGpio_SetDataDirection(&GpioInput, SWITCH_CHANNEL, 0xF);

	while (1) 
	{
		// Read the state of the data so that it can be verified
		Data = XGpio_DiscreteRead(&GpioInput, SWITCH_CHANNEL);
		//xil_printf("Data = 0x%x\n", Data);
		XGpio_DiscreteWrite(&GpioOutput, LED_CHANNEL, Data);
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

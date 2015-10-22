#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xgpio.h"

#define SWITCH_CHANNEL	1
#define MY_DELAY 1000000


XIntc intc;
void GpioHandler(void* CallbackRef)
{
	XGpio* GpioPtr = (XGpio*)CallbackRef;

	u32 CurrentISR = XIntc_In32(intc.BaseAddress + XIN_ISR_OFFSET);
	xil_printf("interrupt occured...0x%x\r\n", CurrentISR);
	// Clear the interrupt
	XGpio_InterruptClear(GpioPtr, 0xF);
}

int main(void)
{
	XGpio switch_Gpio;
	int Status;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the GPIO driver. If an error occurs then exit
	Status = XGpio_Initialize(&switch_Gpio, XPAR_DIP_SWITCHES_4BITS_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Set the direction for all signals to be inputs
	XGpio_SetDataDirection(&switch_Gpio, SWITCH_CHANNEL, 0xF);

	// Initialize the interrupt controller driver so that it's ready to use.
	// specify the device ID that was generated in xparameters.h
	Status = XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Hook up interrupt service routine
	XIntc_Connect(&intc, XPAR_MICROBLAZE_0_INTC_DIP_SWITCHES_4BITS_IP2INTC_IRPT_INTR,
	GpioHandler, &switch_Gpio);


	// Enable the interrupt vector at the interrupt controller
	XIntc_Enable(&intc, XPAR_MICROBLAZE_0_INTC_DIP_SWITCHES_4BITS_IP2INTC_IRPT_INTR);


	// Start the interrupt controller such that interrupts are recognized
	// and handled by the processor
	Status = XIntc_Start(&intc, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Enable the GPIO channel interrupts so that dip switch can be
	// detected and enable interrupts for the GPIO device
	XGpio_InterruptEnable(&switch_Gpio, 0xF);
	XGpio_InterruptGlobalEnable(&switch_Gpio);

	// Initialize the exception table and register the interrupt
	// controller handler with the exception table
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
		(Xil_ExceptionHandler)XIntc_InterruptHandler, &intc);

	// Enable non-critical exceptions
	Xil_ExceptionEnable();

	while (1)
	{
		//for(u32 myloop=0; myloop<MY_DELAY; myloop++) __asm volatile ("nop\n");
		//XGpio_Out32(switch_Gpio.BaseAddress+XGPIO_ISR_OFFSET, 0x1);
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

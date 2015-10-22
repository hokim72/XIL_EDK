#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xtmrctr.h"

unsigned int count = 0;
void TimerCounterHandler(void* CallBackRef, u8 TmrCtrNumber)
{
	XTmrCtr* InstancePtr = (XTmrCtr*)CallBackRef;

	if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
		xil_printf("count = %d\r\n", count++);
	}
}

int main(void)
{
	XIntc intc;
	XTmrCtr tmrctr;
	int Status;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the timer counter so that it's ready to use,
	// specify the device ID that is generated in xparameters.h
	Status = XTmrCtr_Initialize(&tmrctr, XPAR_TMRCTR_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Initialize the interrupt controller driver so that 
	// it's ready to use, specify the device ID that is generated in
	// xparameters.h
	Status = XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Connect a device driver handler that will be called when an interrupt
	// for the device occurs, the device driver handler performs the specific
	// interrupt processing for the device
	Status = XIntc_Connect(&intc, XPAR_MICROBLAZE_0_INTC_AXI_TIMER_0_INTERRUPT_INTR, 
			XTmrCtr_InterruptHandler, &tmrctr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Enable the interrupt for the timer counter
	XIntc_Enable(&intc, XPAR_MICROBLAZE_0_INTC_AXI_TIMER_0_INTERRUPT_INTR);

	// Start the interrupt controller such that interrupts are enabled for
	// all devices that cause interrupts, specific real mode so that
	// the timer counter can cause interrupts thru the interrupt controller.
	Status = XIntc_Start(&intc, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Initialize the exception table.
	Xil_ExceptionInit();

	// Register the interrupt controller handler with the exception table.
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
    	(Xil_ExceptionHandler)XIntc_InterruptHandler, &intc);

	// Enable non-critical exceptions.
	Xil_ExceptionEnable();

	// Setup the handler for the timer counter that will be called from the
	// interrupt context when the timer expires, specify a pointer to the
	// timer counter driver instance as the callback reference so the handler
	// is able to access the instance data
	XTmrCtr_SetHandler(&tmrctr, TimerCounterHandler, &tmrctr);

	// Enable the interrupt of the timer counter so interrupts will occur
	// and use auto reload mode such that the timer counter will reload
	// itself automatically and continue repeatedly, without this option
	// it would expire once only
	XTmrCtr_SetOptions(&tmrctr, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION
	| XTC_DOWN_COUNT_OPTION);

	// Set a reset value for the timer counter such that it will expire
	// when it rolls under to 0, the reset value is loaded
	// into the timer counter when it is started
	XTmrCtr_SetResetValue(&tmrctr, 0, XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ-1);

	// Start the timer counter 
	XTmrCtr_Start(&tmrctr, 0);

	while (1) {
	}
	
	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

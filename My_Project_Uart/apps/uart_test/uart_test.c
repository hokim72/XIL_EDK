#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xuartlite.h"

int SetupInterruptSystem(XUartLite* UartLitePtr);
void SendHandler(void* CallBackRef, unsigned int EventData);
void RecvHandler(void* CallBackRef, unsigned int EventData);

XIntc intc;
static volatile int TotalReceivedCount;
static volatile int TotalSentCount;

int main(void)
{
	int Status;
	XUartLite UartLite;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the UartLite driver so that it is ready to use.
	Status = XUartLite_Initialize(&UartLite, XPAR_UART_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Connect the UartLite to the interrupt subsystem such that interrupts
	// can occur. This function is application specific.
	Status = SetupInterruptSystem(&UartLite);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Setup the handlers for the UartLite that will be called from the
	// interrupt context when data has been sent and received, specify a
	// pointer to the UartLite driver instance as the callback reference so
	// that the handlers are able to access the instance data.
	XUartLite_SetSendHandler(&UartLite, SendHandler, &UartLite);
	XUartLite_SetRecvHandler(&UartLite, RecvHandler, &UartLite);

	// Enable the interrupt of the UartLite so that interrupts will occur.
	XUartLite_EnableInterrupt(&UartLite);

	u8 buffer;
	while (1) {
		TotalReceivedCount = 0;
		XUartLite_Recv(&UartLite, &buffer, 1);
		while (TotalReceivedCount != 1);
		TotalSentCount = 0;
		XUartLite_Send(&UartLite, &buffer, 1);
		while (TotalSentCount != 1);
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

int SetupInterruptSystem(XUartLite* UartLitePtr)
{
	int Status;

	// Initialize the interrupt controller driver so that it is ready to use.
	Status = XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Connect a device driver hander that will be called when an interrupt
	// for the device occurs, the device driver handler performs the specific
	// interrupt processing for the device.
	Status = XIntc_Connect(&intc, XPAR_INTC_0_UARTLITE_1_VEC_ID,
		(XInterruptHandler)XUartLite_InterruptHandler,
		(void*)UartLitePtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Start the interrupt controller such that interrupts are enabled for
	// all devices that cause interrupts, specific real mode so that
	// the UartLite can cause interrupts through the interrupt controller.
	Status = XIntc_Start(&intc, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Enable the interrupt for the UartLite device.
	XIntc_Enable(&intc, XPAR_INTC_0_UARTLITE_1_VEC_ID);

	// Initialize the exception table.
	Xil_ExceptionInit();

	// Register the interrupt controller handler with the exception table.
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
		(Xil_ExceptionHandler)XIntc_InterruptHandler,
		&intc);

	// Enable exceptions.
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

void SendHandler(void* CallBackRef, unsigned int EventData)
{
	TotalSentCount = EventData;
}

void RecvHandler(void* CallBackRef, unsigned int EventData)
{
	TotalReceivedCount = EventData;
}

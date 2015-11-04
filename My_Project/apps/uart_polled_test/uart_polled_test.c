#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xuartlite.h"

#define TEST_BUFFER_SIZE 16
u8 Buffer[TEST_BUFFER_SIZE];
int main(void)
{
	int Status;
	unsigned int SentCount;
	unsigned int ReceivedCount = 0;
	XUartLite UartLite;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the UartLite driver so that it is ready to use.
	Status = XUartLite_Initialize(&UartLite, XPAR_USB_UART_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Receive the number of bytes which is trasfered.
	// Data may be received in fifo which some delay hence we continuously
	// check the receive fifo for valid data and update the receive buffer
	// accordingly.
	while (1) {
		ReceivedCount += XUartLite_Recv(&UartLite,
					Buffer + ReceivedCount,
					TEST_BUFFER_SIZE - ReceivedCount);
		if (ReceivedCount == TEST_BUFFER_SIZE) {
			break;
		}
	}


	SentCount = XUartLite_Send(&UartLite, Buffer, TEST_BUFFER_SIZE);
	if (SentCount != TEST_BUFFER_SIZE) {
		return XST_FAILURE;
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

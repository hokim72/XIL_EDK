#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xspi.h"

int main(void)
{
	XSpi spi;
	int Status;
	u32 StatusReg;
	u32 ControlReg;
	u8 inBuffer, outBuffer;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the SPI driver so that it's ready to use,
	// specify the device ID that is generated in xparameters.h
	Status = XSpi_Initialize(&spi, XPAR_SPI_FLASH_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	//xil_printf("XipMode = %d\r\n", spi.XipMode); // 0
	//xil_printf("HasFifos = %d\r\n", spi.HasFifos); // 1
	//xil_printf("SpiMode = %d\r\n", spi.SpiMode); // 0
	//xil_printf("SlaveOnly = %d\r\n", spi.SlaveOnly); // 0
	//xil_printf("DataWidth = %d\r\n", spi.DataWidth); // 8

	// Setup the control register to enable master mode and the loopback
	// so that data can be sent and received
	Status = XSpi_SetOptions(&spi, XSP_MASTER_OPTION | XSP_LOOPBACK_OPTION);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	
	// We do not need interrupts for this loopback test.
	XSpi_IntrGlobalDisable(&spi);

	// Send data up to maximum size of the transmit register, which is
	// one byte without FIFOs. We send data 4 times just to exercise the
	// device through more than one iteration.
	for (int i=0; i<4; i++) {
		outBuffer = i;
		StatusReg = XSpi_GetStatusReg(&spi);
		//while ((StatusReg & XSP_SR_TX_FULL_MASK) == 0) {
			XSpi_WriteReg(spi.BaseAddr, XSP_DTR_OFFSET, outBuffer);
			//StatusReg = XSpi_GetStatusReg(&spi);
		//}
		xil_printf("send : %d\r\n", i); 

		// Start the tranfer by not inhibiting the transmitter and
		// enabling the device.
		ControlReg = XSpi_GetControlReg(&spi) & (~XSP_CR_TRANS_INHIBIT_MASK);
		XSpi_SetControlReg(&spi, ControlReg | XSP_CR_ENABLE_MASK);

		// Wait for the transfer to be done by polling the transmit
		// empty status bit.
		do {
			StatusReg = XSpi_GetStatusReg(&spi);
		} while ((StatusReg & XSP_SR_TX_EMPTY_MASK) == 0);

		// Receive and verify the data just transmitted.
		//while ((StatusReg & XSP_SR_RX_EMPTY_MASK) == 0) {
			inBuffer = XSpi_ReadReg(spi.BaseAddr, XSP_DRR_OFFSET);
			//StatusReg = XSpi_GetStatusReg(&spi);
		//}
		xil_printf("receive : %d\r\n", inBuffer); 

		// Stop the transfer (hold off automatic sending) by inhibiting
		// the transmitter and disabling the device.
		ControlReg |= XSP_CR_TRANS_INHIBIT_MASK;
		XSpi_SetControlReg(&spi, ControlReg & ~XSP_CR_ENABLE_MASK);
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

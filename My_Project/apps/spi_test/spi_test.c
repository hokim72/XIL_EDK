#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xspi.h"

int main(void)
{
	XSpi spi;
	int Status;
	u8 sendBuffer[5], recvBuffer[5];

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the device and verify success
	Status = XSpi_Initialize(&spi, XPAR_SPI_FLASH_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("FATAL: Cannot locate system SPI device. Exiting...\r\n");
		return Status;
	}

	// Set the SPI device options for Master Mode and manual slave select
	Status = XSpi_SetOptions(&spi, XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION);
	if (Status != XST_SUCCESS) {
		xil_printf("FATAL: SPI options could not be set.\r\n");
		return Status;
	}

	// Start the SPI device controller. Not much point cheking the return code,
	// since its either XST_SUCCESS or XST_DEVICE_IS_STARTED.
	XSpi_Start(&spi);
	
	// We will use the SPI driver in polled mode, so disable the Global Interrupt flag.
	// This must be done AFTER the controller has been started.
	XSpi_IntrGlobalDisable(&spi);

	// Select flash device in the controller.
	// This must be done AFTER the controller has been started.
	Status = XSpi_SetSlaveSelect(&spi, 0x01);
	if (Status != XST_SUCCESS) {
		xil_printf("FATAL: SPI device selection failed!\r\n");
		return Status;
	}

	sendBuffer[0] = 0x9F; // Read Identification
	Status = XSpi_Transfer(&spi, sendBuffer, recvBuffer, 5);

	switch (Status)
	{
		case XST_DEVICE_IS_STOPPED:
			xil_printf("SPI XFER ERROR - SPI device is not started.\r\n");
			break;
		case XST_DEVICE_BUSY:
			xil_printf("SPI XFER ERROR - SPI device is already performing a transfer.\r\n");
			break;
		case XST_SPI_NO_SLAVE:
			xil_printf("SPI XFER ERROR - No device is selected as a slave to this master.\r\n");
			break;
		default:
			// XST_SUCCESS
			break;
	}

	xil_printf("Manufacturer Code = : %02x \r\n", recvBuffer[1]);
	xil_printf("Memory Type       = : %02x \r\n", recvBuffer[2]);
	xil_printf("Memory Capacity   = : %02x \r\n", recvBuffer[3]);
	xil_printf("EDID+CFD length   = : %02x \r\n", recvBuffer[4]);

	xil_printf("N25Q128   = 20, BA, 18, 10\r\n");

	XSpi_Stop(&spi);

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

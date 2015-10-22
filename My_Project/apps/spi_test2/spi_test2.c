#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xspi.h"

int main(void)
{
	XSpi_Config* spiCfg;
	XSpi spi;
	int status;
	u8 inBuffer, outBuffer;
	
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the SPI driver so that it is ready to use.
	spiCfg = XSpi_LookupConfig(XPAR_SPI_FLASH_DEVICE_ID);
	if (spiCfg == XNULL) {
		return XST_FAILURE;
	}
	status = XSpi_CfgInitialize(&spi, spiCfg, spiCfg->BaseAddress);
	if (status != XST_SUCCESS) {
		return status;
	}

	// Set the Spi device as a master.
	status = XSpi_SetOptions(&spi, XSP_MASTER_OPTION | XSP_LOOPBACK_OPTION);
	if (status != XST_SUCCESS) {
		return status;
	}

	// Start the Spi driver
	status = XSpi_Start(&spi);
	if (status != XST_SUCCESS) {
		return status;
	}

	// Disable Global interrupt to use polled mode operation
	status = XSpi_IntrGlobalDisable(&spi);
	if (status != XST_SUCCESS) {
		return status;
	}

	status = XSpi_SetSlaveSelect(&spi, 0x01);
	if (status != XST_SUCCESS) {
		return status;
	}

	for (int i=0; i<4; i++) {
		outBuffer = i;
		status = XSpi_Transfer(&spi, &outBuffer, NULL, 1);
		if (status != XST_SUCCESS) {
			return status;
		}
		xil_printf("send : %d\r\n", outBuffer);

		status = XSpi_Transfer(&spi, &outBuffer, &inBuffer, 1);
		if (status != XST_SUCCESS) {
			return status;
		}
		xil_printf("receive : %d\r\n", inBuffer);
	}
	status = XSpi_Stop(&spi);

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;

}

#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xspi.h"
#include "xuartlite_l.h"

// Instruction Set -- from data sheet(25LC256)
#define EEPROM_READ     0x03
#define EEPROM_WRITE    0x02
#define EEPROM_WRDI     0x04
#define EEPROM_WREN     0x06
#define EEPROM_RDSR     0x05
#define EEPROM_WRSR     0x01

// EEPROM Status Register Bits -- from data sheet(25LC256)
// Use these to parse status register
#define EEPROM_WRITE_IN_PROGRESS	0
#define EEPROM_WRITE_ENABLE_LATCH	1
#define EEPROM_BLOCK_PROTECT_0		2
#define EEPROM_BLOCK_PROTECT_1		3

#define EEPROM_BYTES_PER_PAGE		64
#define	EEPROM_BYTES_MAX			0x7FF

XSpi spi;

int EEPROM_readByte(u16 address, u8* data);
int EEPROM_readWord(u16 address, u16* data);
int EEPROM_readStatus(u8* data);
int EEPROM_writeEnable(void);
int EEPROM_writeByte(u16 address, u8 data);
int EEPROM_writeWord(u16 address, u16 data);
int EEPROM_clearAll(void);

u8 getNumber(void);

int main(void)
{
	int Status;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the device and verify success
	Status = XSpi_Initialize(&spi, XPAR_SPI_EEPROM_DEVICE_ID);
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

	// Start the SPI device controller. Not much point checking the return code,
	// since its either XST_SUCCESS or XST_DEVICE_IS_STARTED.
	XSpi_Start(&spi);

	// We will use the SPI driver in polled mode, so disable the Global Interrupt flag.
	// This must be done AFTER the controller has been started.
	XSpi_IntrGlobalDisable(&spi);

	// Select flash device in the controller.
	Status = XSpi_SetSlaveSelect(&spi, 0x01);
	if (Status != XST_SUCCESS) {
		xil_printf("FATAL: SPI device selection failed!\r\n");
		return Status;
	}

	while (1) {
		xil_printf("\r\n====  EEPROM Memory Playground ====\r\n");
		xil_printf("Address  Value\r\n");

		for (int i=0; i<10; i++)
		{
			u8 data;
			EEPROM_readByte(i, &data);
			xil_printf("  %04x    %02x\r\n", i, data);
		}

		xil_printf(" [e] to erase all memory\r\n");
		xil_printf(" [w] to write byte to memory\r\n");
		xil_printf(" [q] to quit\r\n\r\n");

		switch (XUartLite_RecvByte(XPAR_USB_UART_BASEADDR))
		{
			case 'e':
				xil_printf("Clearing EEPROM, this could take a few seconds.\r\n");
				EEPROM_clearAll();
				break;
			case 'w':
				xil_printf("Which memory slot would you like to write to?\r\n");
				u16 address = getNumber();
				xil_printf("\r\nWhat number would you like to store there?\r\n");
				u8 i = getNumber();
				EEPROM_writeByte(address, i);
				xil_printf("\r\n");
				break;
			case 'q':
				goto quit;
			default:
				xil_printf("What??\r\n");

		}
	}
	quit:
	XSpi_Stop(&spi);
	xil_printf("Good bye!!\r\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

int EEPROM_readByte(u16 address, u8* data)
{
	int Status;
	u8 sendBuffer[4], recvBuffer[4];
	sendBuffer[0] = EEPROM_READ;
	sendBuffer[1] = (address >> 8) & 0xFF;
	sendBuffer[2] = address & 0xFF;

	Status = XSpi_Transfer(&spi, sendBuffer, recvBuffer, 4);
	if (Status != XST_SUCCESS) {
		xil_printf("EEPROM_readByte: SPI transfer failed!\r\n");
		return Status;
	}

	*data = recvBuffer[3];

	return XST_SUCCESS;
}

int EEPROM_readWord(u16 address, u16* data)
{
	int Status;
	u8 sendBuffer[5], recvBuffer[5];
	sendBuffer[0] = EEPROM_READ;
	sendBuffer[1] = (address >> 8) & 0xFF;
	sendBuffer[2] = address & 0xFF;

	Status = XSpi_Transfer(&spi, sendBuffer, recvBuffer, 5);
	if (Status != XST_SUCCESS) {
		xil_printf("EEPROM_readWord: SPI transfer failed!\r\n");
		return Status;
	}

	*data = recvBuffer[3] << 8;
	*data += recvBuffer[4];

	return XST_SUCCESS;
}

int EEPROM_readStatus(u8* data)
{
	int Status;
	u8 sendBuffer[2], recvBuffer[2];
	sendBuffer[0] = EEPROM_RDSR;

	Status = XSpi_Transfer(&spi, sendBuffer, recvBuffer, 2);
	if (Status != XST_SUCCESS) {
		xil_printf("EEPROM_readStatus: SPI transfer failed!\r\n");
		return Status;
	}
	*data = recvBuffer[1];

	return XST_SUCCESS;
}

int EEPROM_writeEnable()
{
	int Status;
	u8 sendBuffer;
	sendBuffer= EEPROM_WREN;
	Status = XSpi_Transfer(&spi, &sendBuffer, NULL, 1);
	if (Status != XST_SUCCESS) {
		xil_printf("EEPROM_writEnable: SPI transfer failed!\r\n");
		return Status;
	}
	return XST_SUCCESS;
}

int EEPROM_writeByte(u16 address, u8 data)
{
	int Status;
	u8 sendBuffer[4];

	EEPROM_writeEnable();

	sendBuffer[0] = EEPROM_WRITE;
	sendBuffer[1] = (address>>8) & 0xFF;
	sendBuffer[2] = address & 0xFF;
	sendBuffer[3] = data;

	Status = XSpi_Transfer(&spi, sendBuffer, NULL, 4);
	if (Status != XST_SUCCESS) {
		xil_printf("EEPROM_writeByte: SPI transfer failed!\r\n");
		return Status;
	}

	u8 statusRegister;
	do {
		EEPROM_readStatus(&statusRegister);
	} while( statusRegister & (1<<EEPROM_WRITE_IN_PROGRESS));

	return XST_SUCCESS;
}

int EEPROM_writeWord(u16 address, u16 data)
{
	int Status;
	u8 sendBuffer[5];

	EEPROM_writeEnable();

	sendBuffer[0] = EEPROM_WRITE;
	sendBuffer[1] = (address>>8) & 0xFF;
	sendBuffer[2] = address & 0xFF;
	sendBuffer[3] = (data>>8) & 0xFF;
	sendBuffer[4] = data & 0xFF;

	Status = XSpi_Transfer(&spi, sendBuffer, NULL, 6);
	if (Status != XST_SUCCESS) {
		xil_printf("EEPROM_writeWord: SPI transfer failed!\r\n");
		return Status;
	}

	u8 statusRegister;
	do {
		EEPROM_readStatus(&statusRegister);
	} while( statusRegister & (1<<EEPROM_WRITE_IN_PROGRESS));

	return XST_SUCCESS;
}

int EEPROM_clearAll(void)
{
	int Status;
	u8 i;
	u16 pageAddress = 0;
	u8 sendBuffer[EEPROM_BYTES_PER_PAGE+3];

	for (i=0; i<EEPROM_BYTES_PER_PAGE; i++) sendBuffer[3+i] = 0;

	while (pageAddress < EEPROM_BYTES_MAX)
	{
		EEPROM_writeEnable();

		sendBuffer[0] = EEPROM_WRITE;
		sendBuffer[1] = (pageAddress>>8) & 0xFF;
		sendBuffer[2] = pageAddress & 0xFF;
		Status = XSpi_Transfer(&spi, sendBuffer, NULL, EEPROM_BYTES_PER_PAGE+3);
		if (Status != XST_SUCCESS) {
			xil_printf("EEPROM_clearAll: SPI transfer failed!\r\n");
			return Status;
		}

		u8 statusRegister;
		do {
			EEPROM_readStatus(&statusRegister);
		} while( statusRegister & (1<<EEPROM_WRITE_IN_PROGRESS));

		pageAddress += EEPROM_BYTES_PER_PAGE;
	}

	return XST_SUCCESS;
}

u8 getNumber()
{
	char hundreds = '0';
	char tens = '0';
	char ones = '0';
	char thisChar = '0';
	do {
		hundreds = tens;
		tens = ones;
		ones = thisChar;
		thisChar = XUartLite_RecvByte(XPAR_USB_UART_BASEADDR);
		XUartLite_SendByte(XPAR_USB_UART_BASEADDR, thisChar);
	} while (thisChar != '\r');
	XUartLite_RecvByte(XPAR_USB_UART_BASEADDR);
	u8 number = (100 * (hundreds - '0') + 10 * (tens - '0') + ones - '0');
	return number;
}

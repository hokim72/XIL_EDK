#include "xspi.h"
#include "SF_commands.h"

void poll_until_complete(XSpi* mySpi)
{
	u8 status_reg;

	// Poll the SF status register and check the WIP bit
	do
	{
		status_reg = SF_read_status_register(mySpi);
	}
	while((status_reg & WRITE_IN_PROGRESS_MASK) == WRITE_IN_PROGRESS_MASK);
}

void SF_bulk_erase(XSpi* mySpi)
{
	u8 op_code = BE;

	// Send the Write Enable op-code
	SF_write_enable(mySpi);

	// Send the Bulk Erase op-code -- ignoring recv_data
	spi_transfer(mySpi, &op_code, NULL, sizeof(op_code));

	// Wait for the Erase operation to finish.
	poll_until_complete(mySpi);

	// Send the Write Disable op-code
	SF_write_disable(mySpi);
}

void SF_page_program(XSpi* mySpi, u8 sector_address, u8 page_address, u8* send_data)
{
	// Send the Write Enable op-code
	SF_write_enable(mySpi);

	// Send the Page Program op-code -- ignoring recv_data
	send_data[0] = PP;
	send_data[1] = sector_address;
	send_data[2] = page_address;
	send_data[3] = 0;

	spi_transfer(mySpi, send_data, NULL, CMD_LEN+SF_BYTES_PER_PAGE);

	// Wait for the Write operation to finish.
	poll_until_complete(mySpi);

	// Send the Write Disable op-code
	SF_write_disable(mySpi);
}

void SF_read(XSpi* mySpi, u16 sector_address, u16 page_address, u32 buflen,
				u8* send_data, u8* recv_data)
{
	// Prime the SPI read command sequence.
	send_data[0] = READ;
	send_data[1] = sector_address;
	send_data[2] = page_address;
	send_data[3] = 0;

	// Send the Read command to the SPI device.
	spi_transfer(mySpi, send_data, recv_data, buflen);
}

void SF_read_id(XSpi* mySpi)
{
	u8 send_data[ID_LEN], recv_data[ID_LEN];

	send_data[0] = RDID;

	// Transfer the op-code and retrieve the ID
	spi_transfer(mySpi, send_data, recv_data, ID_LEN);

	// Send the results to the console.
	xil_printf("Manufacturer Code = : %02x \r\n", recv_data[1]);
	xil_printf("Memory Type       = : %02x \r\n", recv_data[2]);
	xil_printf("Memory Capacity   = : %02x \r\n", recv_data[3]);
	xil_printf("EDID+CFD length   = : %02x \r\n", recv_data[4]);

	xil_printf("N25Q128 = 20, BA, 18, 10\r\n");
}

u8 SF_read_status_register(XSpi* mySpi)
{
	u8 status_reg[SR_LEN], op_code[SR_LEN];

	op_code[0] = RDSR;

	// Read the Status Register
	spi_transfer(mySpi, op_code, status_reg, SR_LEN);

	return status_reg[1];
}

void SF_sector_erase(XSpi* mySpi, u8 sector_address)
{
	u8 op_code[CMD_LEN];

	// Send the Write Enable op-code -- ignoring bogus_data
	SF_write_enable(mySpi);

	// Send the Sector Erase op-code -- ignoring recv_data
	op_code[0] = SE;
	op_code[1] = sector_address;
	op_code[2] = 0x00;
	op_code[3] = 0x00;
	spi_transfer(mySpi, op_code, NULL, CMD_LEN);

	poll_until_complete(mySpi);
	SF_write_disable(mySpi);
}

void SF_write_disable(XSpi* mySpi)
{
	u8 op_code = WRDI;

	// Send the Write Disable op-code -- ignoring recv_data
	spi_transfer(mySpi, &op_code, NULL, sizeof(op_code));
}

void SF_write_enable(XSpi* mySpi)
{
	u8 op_code = WREN;

	// Send the Write Enable op-code -- ignoring recv_data
	spi_transfer(mySpi, &op_code, NULL, sizeof(op_code));
}

void slave_select(XSpi* mySpi, u32 slaveMask)
{
	int retCode;

	retCode = XSpi_SetSlaveSelect(mySpi, slaveMask);

	switch (retCode)
	{
		case XST_DEVICE_BUSY:
			print("DEVICE BUSY - SPI device is already performing a transfer.\r\n");
			break;
		case XST_SPI_TOO_MANY_SLAVES:
			print("TOO MANY SLAVES - Only one slave at a time can be selected.\r\n");
			break;
		default:
			// XST_SUCCESS
			break;
	}
}

void spi_transfer(XSpi* mySpi, unsigned char* send, unsigned char* recv, u32 num_bytes)
{
	u32 retCode;

	// If no data was designated then return an error.
	if (num_bytes == 0)
	{
		print("SPI XFER ERROR - no bytes to send.\r\n");
		return;
	}

	retCode = XSpi_Transfer(mySpi, send, recv, num_bytes);

	switch (retCode)
	{
		case XST_DEVICE_IS_STOPPED:
			print("SPI XFER ERROR - SPI device is not started.\r\n");
			break;
		case XST_DEVICE_BUSY:
			print("SPI XFER ERROR - SPI device is already performing a transfer.\r\n");
			break;
		case XST_SPI_NO_SLAVE:
			print("SPI XFER ERROR - No device is selected as a slave to this master.\r\n");
			break;
		default:
			// XST_SUCCESS
			break;
	}
}

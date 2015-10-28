#ifdef USE_FS
#include <mfs_config.h> // For memory file system definitions.
#endif // USE_FS
#include <stdlib.h> // For exit()
#include "xparameters.h"
#include "xil_cache.h"
#include "xbasic_types.h"
#include "xspi.h"

#include "SF_commands.h"

#define APP_DESTINATION_ADDR	XPAR_MCB3_LPDDR_S0_AXI_BASEADDR

// If a file system is to used, get the destination address from the
// MFS configuration
#ifdef USE_FS
#define FS_DESTINATION_ADDR		MFS_BASE_ADDRESS
#endif // USE_FS

// Define the application storage area of the flash.
// Corresponds to maximum program size. 0x100000 = 640KB.
#define APP_SECTOR_START	16	// 16 * 256 bytes/page * 256 pages/sector = 0x100000
#define APP_SECTOR_END	    32  // 32 * 256 bytes/page * 256 pages/sector = 0x200000

// Define the file system storage area of the flash.
#define FS_SECTOR_START		32  // 32 * 256 bytes/page * 256 pages/sector = 0x200000
#define FS_SECTOR_END		64  // 64 * 256 bytes/page * 256 pages/sector = 0x400000

// Define the microblaze vector storage area of the flash.
#define VECTOR_SECTOR		64   // 64 * 256 bytes/page * 256 pages/sector = 0x400000
#define VECTOR_SIZE			(0x50 + CMD_LEN) // Corressponds to MicroBlaze vector size.
#define VECTOR_ADDR			0x0  // Corresponds to the target destination offset in memory.

// Maximum send/receive buffer length in bytes
#define BUF_LEN				(SF_BYTES_PER_PAGE + CMD_LEN)

XSpi spiInstance; // SPI configuration space.
XSpi *configSPIptr; // SPI controller driver instance pointer.

int (*boot_app) (void);

int main(void)
{
	u8 recv_data[BUF_LEN];
	u8 send_data[BUF_LEN];
	u16 offset;
	u16 page;
	u16 sector;
	u32 result;
	u8* destination_location = (unsigned char *) APP_DESTINATION_ADDR;

	// Enable cache and initialize it.
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	xil_printf("\r\nAvnet S6-LX9 Booting...\r\n");

	// Set the instance pointer to the SPI configuration space allocated for this device.
	configSPIptr = &spiInstance;

	// Initialize the device and verify success.
	result = XSpi_Initialize(configSPIptr, XPAR_SPI_FLASH_DEVICE_ID);

	if (result != XST_SUCCESS)
	{
		xil_printf("FATAL: Cannot locate system SPI device. Exiting...\r\n");
		exit(1);
	}

	// Set the SPI device options for Master Mode and manual slave select
	result = XSpi_SetOptions(configSPIptr, XSP_MASTER_OPTION |
									XSP_MANUAL_SSELECT_OPTION);

	if (result != XST_SUCCESS)
	{
		xil_printf("FATAL: SPI options could not be set.\r\n");
		exit(1);
	}

	// Start the SPI device controller. Not much point checking the return code,
	// since its either XST_SUCCESS or XST_DEVICE_IS_STARTED.
	(void) XSpi_Start(configSPIptr);

	// We will use the SPI driver in polled mode, so disable the Global Interrupt flag.
	// This must be done AFTER the controller has been started.
	XSpi_IntrGlobalDisable(configSPIptr);

	// Select flash device in the controller.
	// This must be done AFTER the controller has been started.
	result = XSpi_SetSlaveSelect(configSPIptr, SPI_FLASH_SELECT);
	if (result != XST_SUCCESS)
	{
		xil_printf("FATAL: SPI device selection failed!\r\n");
		exit(1);
	}

	// Get the SPI flash manufacturer ID and display to the user for debug.
#ifdef BOOT_DEBUG
	SF_read_id(configSPIptr);
#endif // BOOT_DEBUG

	// Read flash and copy application image to RAM.

	// Copy APP_SIZE number of bytes from SPI flash to system RAM.
#ifdef BOOT_DEBUG
	xil_printf("Application Loading...");
#endif // BOOT_DEBUG
	sector = APP_SECTOR_START;
	page = 0;
	destination_location = (unsigned char *)APP_DESTINATION_ADDR;

	while (sector < APP_SECTOR_END)
	{
		// Instruct the SPI controller to read the from the current sector
		// and page in the SPI flash.
		SF_read(configSPIptr, sector, page, BUF_LEN, send_data, recv_data);

		// Copy all bytes read into RAM.
		for (offset = CMD_LEN; offset< (u16)BUF_LEN; offset++)
		{
			*destination_location++ = recv_data[offset];
		}

		// Check to see if the end of the current sector has been reached.
		if (page++ >= SF_PAGES_PER_SECTOR)
		{
			// The final page of this sector has been reached, move onto the
			// first page of the next sector.
			page = 0;
			sector++;

			// Show some progress of the boot process.
			xil_printf(".");
		}
	}
#ifdef BOOT_DEBUG
	xil_printf("DONE\r\n");
#endif // BOOT_DEBUG
	
#ifdef VERI
	// Verify application code was copied correctly (checking only first
	// 20000 bytes) from SPI flash to system RAM.
#ifdef BOOT_DEBUG
	xil_printf("Application Verifying...");
#endif // BOOT_DEBUG
	sector = APP_SECTOR_START;
	page = 0;
	destination_location = (unsigned char*)APP_DESTINATION_ADDR;

	while (sector < (APP_SECTOR_START + 2))
	{
		// Instruct the SPI controller to read the from the current sector
		// and page in the SPI flash
		SF_read(configSPIptr, sector, page, BUF_LEN, send_data, recv_data);

		// Verify all bytes exist in RAM.
		for (offset = CMD_LEN; offset < (u16)BUF_LEN; offset++)
		{
			if (*destination_location++ != recv_data[offset])
			{
				xil_printf("ERROR! Application not copied correctly\r\n");
			}
		}

		// Check to see if the end of the current sector has been reached.
		if (++page >= SF_PAGES_PER_SECTOR)
		{
			// The final page of this sector has been reached, move onto the
			// first page of the next sector.
			page = 0;
			sector++;

			// Show some progress of the boot process.
			xil_printf(".");
		}
	}
#ifdef BOOT_DEBUG
	xil_printf("DONE\r\n");
#endif // BOOT_DEBUG

#endif // VERI

	// If a file system is to be used, read flash and copy file system image
	// to RAM.
#ifdef USE_FS
	// Read flash and copy file system image to RAM.
#ifdef BOOT_DEBUG
	xil_printf("File System Loading...");
#endif // BOOT_DEBUG

	// Copy FS_SIZE number of bytes from SPI flash to system RAM.
	sector = FS_SECTOR_START;
	page = 0;
	destination_location = (unsigned char*) (FS_DESTINATION_ADDR);

	while (sector < FS_SECTOR_END)
	{
		// Instruct the SPI controller to read the from the current sector
		// and page in the SPI flash.
		SF_read(configSPIptr, sector, page, BUF_LEN, send_data, recv_data);

		// Copy all bytes read into RAM.
		for (offset = CMD_LEN; offset < (u16)BUF_LEN; offset++)
		{
			*destination_location++ = recv_data[offset];
		}

		// Check to see if the end of the current sector has been reached.
		if (++page >= SF_PAGES_PER_SECTOR)
		{
			// The final page of this sector has been reached, move onto the
			// first page of the next sector.
			page = 0;
			sector++;

			// Show some progress of the boot process.
			xil_printf(".");
		}
	}
#ifdef BOOT_DEBUG
	xil_printf("DONE\r\n");
#endif // BOOT_DEBUG

#ifdef VERI
	// Verify file system image was copied correctly (checking only first
	// 20000 bytes) from SPI flash to system RAM.
#ifdef BOOT_DEBUG
	xil_printf("File System Verifying...");
#endif // BOOT_DEBUG

	sector = FS_SECTOR_START;
	page = 0;
	destination_location = (unsigned char*) FS_DESTINATION_ADDR;

	while (sector < (FS_SECTOR_START + 2))
	{
		// Instruct the SPI controller to read from the current sector
		// and page in the SPI flash.
		SF_read(configSPIptr, sector, page, BUF_LEN, send_data, recv_data);

		// Veriry all bytes exist in RAM.
		for (offset = CMD_LEN; offset < (u16)BUF_LEN; offset++)
		{
			if (*destination_location++ != recv_data[offset])
			{
				xil_printf("\r\nERROR! File system not copied correctly\r\n");
			}
		}

		// Check to see if the end of the current sector has been reached.
		if (++page >= SF_PAGES_PER_SECTOR)
		{
			// The final page of this sector has been reached, move onto the
			// first page of the next sector.
			page = 0;
			sector++;

			// Show some progress of the boot process.
			xil_printf(".");
		}
	}
#ifdef BOOT_DEBUG
	xil_printf("DONE\r\n");
#endif // BOOT_DEBUG

#endif // VERI

#endif // USE_FS

	// Read flash and copy vector image to RAM.
#ifdef BOOT_DEBUG
	xil_printf("Interrupt Vectors Loading...");
#endif // BOOT_DEBUG

	// Copy VECTOR_SIZE number of bytes from SPI flash to system BRAM.
	sector = VECTOR_SECTOR;
	page = 0;
	destination_location = (unsigned char*) VECTOR_ADDR;

	// Instruct the SPI controller to read from the current sector
	// and page in the SPI flash.
	SF_read(configSPIptr, sector, page, VECTOR_SIZE, send_data, recv_data);

	// Copy all bytes read into BRAM.
	for (offset = CMD_LEN; offset < (u16)VECTOR_SIZE; offset++)
	{
		*destination_location++ = recv_data[offset];
	}
#ifdef BOOT_DEBUG
	xil_printf("DONE\r\n");
#endif // BOOT_DEBUG

#ifdef VERI
	// Verify VECTOR_SIZE number of bytes from SPI flash against system BRAM.
#ifdef BOOT_DEBUG
	xil_printf("Interrupt Vector Verifying...");
#endif // BOOT_DEBUG

	sector = VECTOR_SECTOR;
	page = 0;
	destination_location = (unsigned char*) VECTOR_ADDR;

	// Instruct the SPI controller to read from the current sector
	// and page in the SPI flash.
	SF_read(configSPIptr, sector, page, VECTOR_SIZE, send_data, recv_data);

	// Verify all bytes exist in RAM.
	for (offset = CMD_LEN; offset < (u16)VECTOR_SIZE; offset++)
	{
		if (*destination_location++ != recv_data[offset])
		{
			xil_printf("\r\nERROR! Vector not copied correctly\r\n");
		}
	}
#ifdef BOOT_DEBUG
	xil_printf("DONE\r\n");
#endif // BOOT_DEBUG

#ifdef BOOT_DEBUG
	xil_printf("DONE\r\n");
#endif // BOOT_DEBUG

#endif

	// All loading from the SPI flash is complete, ready to complete boot
	// sequence by jumping to application code in system RAM.
	xil_printf("DONE\r\n");

	// Disable cache and reinitialize it so that other applications can be
	// run with no problems.
	Xil_DCacheDisable();
	Xil_ICacheDisable();

	// Update the function pointer that is set to point to the address of
	// DESTINATION_ADDR. This will be used to change execution to the code
	// now residing in system RAM.
	boot_app = (int (*) (void)) APP_DESTINATION_ADDR;

	// Jump to start execution code at the address START_ADDR.
	boot_app();

	// Code execution should never reach here.
	while(1) {;}

	return 0;
}

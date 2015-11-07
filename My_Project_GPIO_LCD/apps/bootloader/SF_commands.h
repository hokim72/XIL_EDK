#include "xspi.h"

void poll_until_complete(XSpi* mySpi);
void SF_bulk_erase(XSpi* mySpi);
void SF_read(XSpi* mySpi, u16 sector_address, u16 page_address, 
				u32 buflen, u8* send_data, u8* recv_data);

void SF_read_id(XSpi* mySpi);
u8 SF_read_status_register(XSpi* mySpi);
void SF_sector_erase(XSpi* mySpi, u8 sector_address);
//void SF_write_status_register(XSpi* mySpi, u8 data);
void SF_write_enable(XSpi* mySpi);
void SF_write_disable(XSpi* mySpi);
void slave_select(XSpi* mySpi, u32 slaveMask);
void spi_transfer(XSpi* mySpi, unsigned char* send, unsigned char* recv, u32 num_bytes);

//	                    | Sectors  |  Pages/Sector  |  Bytes/Page  |  Total Bytes  |
// Intel 64 Mbit S33    |   128    |      256       |     256      |    8388608    |
// STMicro M25P16       |   32     |      256       |     256      |    2097152    |
// Spansion S25FL128P   |   256    |      256       |     256      |   16777216    |
// Spansion S25FL032P   |    64    |      256       |     256      |    4194304    |
// Numonyx N25Q128      |   256    |      256       |     256      |   16777216    |
#define SF_SECTORS				256
#define SF_PAGES_PER_SECTOR		256
#define SF_BYTES_PER_PAGE		256
#define SF_BYTES_PER_SECTOR		(SF_PAGES_PER_SECTOR * SF_BYTES_PER_PAGE)
#define SF_BYTES				(SF_SECTORS * SF_PAGES_PER_SECTOR * SF_BYTES_PER_PAGE)

// Slave Select Masks
#define SPI_NONE_SELECT			0x00
#define SPI_SELECT_0			0x01
#define SPI_SELECT_1			0x02
#define SPI_SELECT_2			0x04
#define SPI_SELECT_3			0x08
#define SPI_SELECT_4			0x10
#define SPI_SELECT_5			0x20
#define SPI_SELECT_6			0x40
#define SPI_SELECT_7			0x80

#define SPI_FLASH_SELECT		SPI_SELECT_0

// SF One-byte Op-Codes
#define WREN				0x06 // Write Enable
#define WRDI				0x04 // Write Disable
#define RDID				0x9F // Read Identification
#define RDSR				0x05 // Read Status Register
#define WRSR				0x01 // Write Status Register
#define READ				0x03 // READ Data Bytes
#define FAST				0x0B // Read Data Bytes at Higher Speed
#define PP					0x02 // Page Program
#define SE					0xD8 // Sector Erase
#define BE					0xC7 // Bulk Erase
#define DP					0xB9 // Deep Power-down
#define RES					0xAB // Read 8-bit Electronic Signature and/or Release from Deep power-down

// SF Status Register masks
#define WRITE_IN_PROGRESS_MASK 0x01

#define DUMMY_BYTE			   0xAA

#define ID_LEN					5 // SPI ID data length in bytes
#define SR_LEN					2 // Status register length in bytes
#define CMD_LEN					4  // Command length in bytes

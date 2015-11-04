#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xintc.h"
#include "xiic.h"

// According to the cdce913 data sheet, the write address is 0xCA and
// the read address is 0xCB. However, the Xilinx IIC API requires that
// only the 7-bit address be passed in so that read/write bit can be set
// within the API itself.
#define CDCE913_I2C_7BIT_ADDRESS		(u8) (0xCA >> 1)

// Define the direct I2C command registers for the CDCE913 clock chip.
// GCR = Generic Configuration Register
// PCR = PLL1 Configuration Register
// Adding a '1' at bit 7 to enable Byte (rather than Block) mode
#define CDCE913_GCR_ID                                     0x80
#define CDCE913_GCR_EEPROM                                 0x81
#define CDCE913_GCR_Y1_A                                   0x82
#define CDCE913_GCR_Y1_B                                   0x83
#define CDCE913_GCR_Y1_C                                   0x84
#define CDCE913_XCSEL                                      0x85
#define CDCE913_BCOUNT                                     0x86
#define CDCE913_SSC1_A                                     0x90
#define CDCE913_SSC1_B                                     0x91
#define CDCE913_SSC1_C                                     0x92
#define CDCE913_FS1                                        0x93
#define CDCE913_MUX_Y2Y3STATE                              0x94
#define CDCE913_Y2Y3_OUTPUT_STATE                          0x95
#define CDCE913_PDIV2                                      0x96
#define CDCE913_PDIV3                                      0x97
#define CDCE913_PLL1_0_A                                   0x98
#define CDCE913_PLL1_0_B                                   0x99
#define CDCE913_PLL1_0_C                                   0x9A
#define CDCE913_PLL1_0_D                                   0x9B
#define CDCE913_PLL1_1_A                                   0x9C
#define CDCE913_PLL1_1_B                                   0x9D
#define CDCE913_PLL1_1_C                                   0x9E
#define CDCE913_PLL1_1_D                                   0x9F

#define CDCE913_EEPIP_MASK                                 0x40

XIntc intc;
XIic iic;
static int SetupInterruptSystem(XIic* IicInstPtr);
static void SendHandler(XIic* IntancePtr);
static void ReceiveHandler(XIic* InstancePtr);
static void StatusHandler(XIic* Instance, int Event);

int cdce913_read_command(u8 address, u8* data);

volatile u8 TransmitComplete;
volatile u8 ReceiveComplete;

int main(void)
{
	int Status;
	u8 command_address;
	u8 command_data;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the IIC driver so that it is read to use.
	Status = XIic_Initialize(&iic, XPAR_CDCE913_I2C_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC initialize failed:%d\n", Status);
		return Status;
	}

	// Setup the Interrupt System.
	Status = SetupInterruptSystem(&iic);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC interrupt setup failed:%d\n", Status);
		return Status;
	}

	// Set the Handlers for transmit and reception.
	XIic_SetSendHandler(&iic, &iic, (XIic_Handler)SendHandler);
	XIic_SetRecvHandler(&iic, &iic, (XIic_Handler)ReceiveHandler);
	XIic_SetStatusHandler(&iic, &iic, (XIic_StatusHandler)StatusHandler);

	// Set the Slave address.
	Status = XIic_SetAddress(&iic, XII_ADDR_TO_SEND_TYPE, 
								CDCE913_I2C_7BIT_ADDRESS);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC set address failed:%d\n", Status);
		return Status;
	}

	command_address = CDCE913_GCR_ID;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_GCR_EEPROM;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_GCR_Y1_A;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_GCR_Y1_B;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_GCR_Y1_C;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_XCSEL;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_BCOUNT;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_SSC1_A;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_SSC1_B;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_SSC1_C;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_FS1;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_MUX_Y2Y3STATE;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_Y2Y3_OUTPUT_STATE;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PDIV2;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PDIV3;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PLL1_0_A;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PLL1_0_B;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PLL1_0_C;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PLL1_0_D;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PLL1_1_A;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PLL1_1_B;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PLL1_1_C;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	command_address = CDCE913_PLL1_1_D;
	Status = cdce913_read_command(command_address, &command_data);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	xil_printf("(0x%02X):    %02X\r\n", (command_address-0x80), command_data);

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

static int SetupInterruptSystem(XIic* IicInstPtr)
{
	int Status;

	// Initialize the interrupt controller driver so that's it's ready to use.
	Status = XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Connect the device driver handler that will be called when an
	// interrupt for the device occurs, the handler defined above performs
	// the specific interrupt processing for the device.
	Status = XIntc_Connect(&intc, XPAR_INTC_0_IIC_0_VEC_ID,
				(XInterruptHandler)XIic_InterruptHandler,
				IicInstPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Start the interrupt controller so interrupts are enabled for all devices
	// that cause interrupts.
	Status = XIntc_Start(&intc, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Enable the interrupts for the IIC device.
	XIntc_Enable(&intc, XPAR_INTC_0_IIC_0_VEC_ID);

	// Initialize the exception table.
	Xil_ExceptionInit();

	// Register the interrupt controller handler with the exception table.
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
		(Xil_ExceptionHandler)XIntc_InterruptHandler, &intc);

	// Enable non-critical exceptions.
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

static void SendHandler(XIic* InstancePtr)
{
	TransmitComplete = 0;
}

static void ReceiveHandler(XIic* InstancePtr)
{
	ReceiveComplete = 0;
}

static void StatusHandler(XIic* InstancePtr, int Event)
{
}

int cdce913_read_command(u8 address, u8* data)
{
	int Status;

	// Start the IIC device.
	Status = XIic_Start(&iic);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC start failed:%d\n", Status);
		return Status;
	}

	TransmitComplete = 1;

	// Send the Data.
	Status = XIic_MasterSend(&iic, &address, 1);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC send failed:%d\n", Status);
		return Status;
	}

	// Wait till the transmission is completed.
	while ((TransmitComplete) || (XIic_IsIicBusy(&iic) == TRUE));

	ReceiveComplete = 1;

	// Receive the Data.
	Status = XIic_MasterRecv(&iic, data, 1);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC receive failed:%d\n", Status);
		return Status;
	}

	// Wait till all the data is received.
	while ((ReceiveComplete) || (XIic_IsIicBusy(&iic) == TRUE));

	// Stop the IIC device.
	Status = XIic_Stop(&iic);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC stop failed:%d\n", Status);
		return Status;
	}

	return XST_SUCCESS;
}

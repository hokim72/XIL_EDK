#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "xiic.h"

// According to the LM75 data sheet, the write address is 0x90 and the read 
// address is 0x91(when A2=A1=A0=0). However, the Xilinx IIC API requires 
// that only the 7-bit address be passed in so that read/write bit can be set
// within the API itself.
#define	LM75_ADDRESS		(u8) (0x90>>1)
#define LM75_TEMP_REGISTER		0x00
#define LM75_CONFIG_REGISTER	0x01
#define LM75_THYST_REGISTER		0x02
#define LM75_TOS_REGISTER		0x03

XTmrCtr DelayTimer;
XIntc intc;
XIic iic;

static void delay_ms(u32 time);
static int SetupInterruptSystem(XIic* IicInstPtr);
static void SendHandler(XIic* InstancePtr);
static void ReceiveHandler(XIic* InstancePtr);
static void StatusHandler(XIic* Instance, int Event);

volatile u8 TransmitComplete;
volatile u8 ReceiveComplete;

int main(void)
{
	int Status;
	u8 sendBuffer, recvBuffer[2];

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the Timer
	Status = XTmrCtr_Initialize(&DelayTimer, XPAR_TMRCTR_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Timer initialization failed:%d\n", Status);
		return Status;
	}
	XTmrCtr_SetOptions(&DelayTimer, 1, XTC_DOWN_COUNT_OPTION);

	// Initialize the IIC driver so that it is read to use.
	Status = XIic_Initialize(&iic, XPAR_LM75_I2C_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC initialization failed:%d\n", Status);
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
	Status = XIic_SetAddress(&iic, XII_ADDR_TO_SEND_TYPE, LM75_ADDRESS);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC set address failed:%d\n", Status);
		return Status;
	}
	
	xil_printf("\r\n====  i2c Thermometer  ====\r\n");

	while (1) {
		// Start the IIC device.
		Status = XIic_Start(&iic);
		if (Status != XST_SUCCESS) {
			xil_printf("IIC send failed:%d\n", Status);
			return Status;
		}

		TransmitComplete = 1;

		sendBuffer = LM75_TEMP_REGISTER;
		Status = XIic_MasterSend(&iic, &sendBuffer, 1);
		if (Status != XST_SUCCESS) {
			xil_printf("IIC receive failed:%d\n", Status);
			return Status;
		}

		// Wait till the transmission is completed.
		while ((TransmitComplete) || (XIic_IsIicBusy(&iic) == TRUE));

		ReceiveComplete = 1;

		// Receive the Data.
		Status = XIic_MasterRecv(&iic, recvBuffer, 2);
		if (Status != XST_SUCCESS) {
			xil_printf("IIC receive failed:%d\n", Status);
			return Status;
		}

		// Wait till all the data is received.
		while ((ReceiveComplete) || (XIic_IsIicBusy(&iic) == TRUE));

		// Stop the IIC device
		Status = XIic_Stop(&iic);
		if (Status != XST_SUCCESS) {
			xil_printf("IIC stop failed:%d\n", Status);
			return Status;
		}

		xil_printf("%d.%d\r\n", recvBuffer[0], (recvBuffer[1] & 0x80) ? 5:0); 

		delay_ms(1000);
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

static void delay_ms(u32 time)
{
	XTmrCtr_SetResetValue(&DelayTimer, 1, time * (XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ/1000));
	XTmrCtr_Start(&DelayTimer, 1);
	while (!(XTmrCtr_IsExpired(&DelayTimer, 1)));
	XTmrCtr_Stop(&DelayTimer, 1);
}

static int SetupInterruptSystem(XIic* IicInstPtr)
{
	int Status;

	// Initialize the interrupt controller driver so that it's ready to use.
	Status = XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	// Connect the device driver handler that will be called when an interrupt
	// for the device occurs, the handler defined above performs the specific
	// interrupt processing for the device.
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

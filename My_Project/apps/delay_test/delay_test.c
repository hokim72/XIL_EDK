#include "xparameters.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xtmrctr.h"

XTmrCtr DelayTimer;

void delay_us(u32 time)
{
	XTmrCtr_SetResetValue(&DelayTimer, 1, time * (XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ/1000000));
	XTmrCtr_Start(&DelayTimer, 1);
	while(!(XTmrCtr_IsExpired(&DelayTimer, 1)));
	XTmrCtr_Stop(&DelayTimer, 1);
}
void delay_ms(u32 time)
{
	XTmrCtr_SetResetValue(&DelayTimer, 1, time * (XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ/1000));
	XTmrCtr_Start(&DelayTimer, 1);
	while(!(XTmrCtr_IsExpired(&DelayTimer, 1)));
	XTmrCtr_Stop(&DelayTimer, 1);
}

int main(void)
{
	int Status;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Initialize the Timer
	Status = XTmrCtr_Initialize(&DelayTimer, XPAR_TMRCTR_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Timer failed to initialize\r\n");
		return Status;
	}
	XTmrCtr_SetOptions(&DelayTimer, 1, XTC_DOWN_COUNT_OPTION);

	int count = 0;
	while (1) {
		delay_ms(1000);
		xil_printf("count = %d\n", count++);
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

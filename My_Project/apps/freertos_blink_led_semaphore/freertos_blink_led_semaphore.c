#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

#include "xil_printf.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xgpio.h"

#define GPIO_DEVICE_ID	XPAR_LEDS_4BITS_DEVICE_ID

#define LED_CHANNEL	1

#define mainLED_TASK_PRIORITY	(tskIDLE_PRIORITY + 1)

#define TIMER_PERIOD 1000
#define TIMER_ID	123

XGpio GpioOutput;

xTimerHandle xTimer;

xTaskHandle xTask;

xSemaphoreHandle xSemaphore_led = NULL;

static void prvLed_Task(void* pvParameters);
int prvSetGpioHardware(void);
void prvShutdown(void);

void vTimerCallback(xTimerHandle pxTimer)
{
	if (xSemaphoreGive(xSemaphore_led) != pdTRUE)
	{
		xil_printf("xSemaphore_led give fail\r\n");
		prvShutdown();
	}
}

int main(void)
{
    microblaze_disable_interrupts();

    #if defined( XPAR_MICROBLAZE_USE_ICACHE ) && ( XPAR_MICROBLAZE_USE_ICACHE != 0 )
    {
        Xil_ICacheInvalidate();
        Xil_ICacheEnable();
    }
    #endif

    #if defined( XPAR_MICROBLAZE_USE_DCACHE ) && ( XPAR_MICROBLAZE_USE_DCACHE != 0 )
    {
        Xil_DCacheInvalidate();
        Xil_DCacheEnable();
    }
    #endif

	// Create Binary Semaphore
	vSemaphoreCreateBinary(xSemaphore_led);
	configASSERT(xSemaphore_led);

	// Setup the GPIO Hardware
	prvSetGpioHardware();

	// Create the task
	xTaskCreate(prvLed_Task, "LED_TASK", configMINIMAL_STACK_SIZE, NULL,
				mainLED_TASK_PRIORITY, &xTask);

	// Create timer. Starting the timer before the scheduler
	// has been started means the timer will start running immediately that
	// the scheduler starts.
	xTimer = xTimerCreate("LedTimer", TIMER_PERIOD, 
			pdTRUE, // auto-reload when expires
			(void*)TIMER_ID, // unique id
			vTimerCallback // Callback
			);
	
	if (xTimer == NULL) {
		// The timer was not created.
		xil_printf("Failed to create timer\r\n");
		prvShutdown();
		return 0;
	} else {
		// Start the timer
		xTimerStart(xTimer, 0);
	}

	// Starting the scheduler will start the timers running as it is already
	// been set into the active state.
	vTaskStartScheduler();

	// Should not reach here.
	for (;;);

	return 0;
}

static void prvLed_Task(void* pvParameters)
{
	unsigned int uiLedFlag = 0;
	static u32 Data = 1;

	for (;;)
	{
		if (xSemaphoreTake(xSemaphore_led, portMAX_DELAY) == pdTRUE)
		{
			uiLedFlag ^= 1;
			if (uiLedFlag == 0) Data<<=1;
			if (Data == 16) Data = 1;
			XGpio_DiscreteWrite(&GpioOutput, LED_CHANNEL, uiLedFlag?Data:0);
		} else {
			xil_printf("xSemaphore_led take fail\r\n");
			// Call shutdown
			prvShutdown();
		}
	}
}

int prvSetGpioHardware(void)
{
	int Status;
	// Initialize the GPIO driver so that it's ready to use,
	// specify the device ID that is generated in xparameters.h
	Status = XGpio_Initialize(&GpioOutput, GPIO_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&GpioOutput, LED_CHANNEL, 0x0);

	// Set the GPIO outputs to low
	XGpio_DiscreteWrite(&GpioOutput, LED_CHANNEL, 0x0);

	return Status;
}

void prvShutdown(void)
{
	// Check if timer is created
	if (xTimer)
		xTimerDelete(xTimer, 0);
	vSemaphoreDelete(xSemaphore_led);
	vTaskDelete(xTask);
}

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "xil_printf.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xgpio.h"

#define SWITCH_CHANNEL	XGPIO_IR_CH1_MASK

xQueueHandle xIntegerQueue, xStringQueue;
XGpio switch_Gpio;

static void vIntegerGenerator(void* pvParameters)
{
	portTickType xLastExecutionTime;
	unsigned long ulValueToSend = 0;
	int i;

	// Initialize the variable used by the call to vTaskDelayUntil().
	xLastExecutionTime = xTaskGetTickCount();

	for (;;)
	{
		// This is a periodic task. Block until it is time to run again.
		// The task will execute every 200ms.
		vTaskDelayUntil(&xLastExecutionTime, 200/portTICK_RATE_MS);

		// Send an incrementing number to the queue five times. The values will
		// be read from the queue by the interrupt service routine. The interrupt
		// service routine always empties the queue so this task is guaranteed
		// to be able to write all five values, so a block time is not required.
		for (i=0; i<5; i++)
		{
			xQueueSendToBack(xIntegerQueue, &ulValueToSend, 0);
			ulValueToSend++;
		}

		// Force an interrupt so the interrupt service routine can read the
		// values from the queue.
		vTaskSuspendAll();
		xil_printf("Generator task - About to generate an interrupt.\n");
		xTaskResumeAll();
		XGpio_Out32(switch_Gpio.BaseAddress+XGPIO_ISR_OFFSET, XGPIO_IR_CH1_MASK);
		vTaskSuspendAll();
		xil_printf("Periodic task - Interrupt generated.\n");
		xTaskResumeAll();
	}
}

static void vStringPrinter(void* pvParameters)
{
	char* pcString;

	for (;;)
	{
		// Block on the queue to wait for data to arrive.
		xQueueReceive(xStringQueue, &pcString, portMAX_DELAY);

		// Print out the string received.
		vTaskSuspendAll();
		xil_printf(pcString);
		xTaskResumeAll();
	}
}

void vSoftwareInterruptHandler()
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	static unsigned long ulReceivedNumber;

	// The strings are declared static const to ensure they are not allocated
	// to the interrupt service routine stack, and exist even when the interrupt
	// service routine is not executing.
	static const char* pcStrings[] =
	{
		"String 0\n",
		"String 1\n",
		"String 2\n",
		"String 3\n"
	};

	// Loop until the queue is empty.
	while (xQueueReceiveFromISR(xIntegerQueue, &ulReceivedNumber,
								&xHigherPriorityTaskWoken) != errQUEUE_EMPTY)
	{
		// Truncate the received value to the last two bits (values 0 to 3 inc.),
		// then send the string that corresponds to the truncated value to the
		// other queue.
		ulReceivedNumber &= 0x03;
		xQueueSendToBackFromISR(xStringQueue, &pcStrings[ulReceivedNumber],
								&xHigherPriorityTaskWoken);
	}

	// Clear teh software interrupt bit using the interrupt controller Clear
	// Pending register.
	XGpio_InterruptClear(&switch_Gpio, SWITCH_CHANNEL);

	// Giving the semaphore may have unblocked a task - if it did and the
	// unblocked task has a priority equal to or above the currently executing
	// task then xHigherPriorityTaskWoken will have been set to pdTRUE and
	// portYIELD_FROM_ISR() will force a context switch to the newly unblocked
	// higher priority task.

	// NOTE: The syntax for forcing a context switch within an ISR varies
	// between FreeRTOS ports. The portYIELD_FROM_ISR() macro is provided as
	// part of the Microblaze port layer for this purpose. taskYIELD() must
	// never be called from ISR!
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vSetupEnvironment(void)
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

	// Initialize the GPIO driver.
	XGpio_Initialize(&switch_Gpio, XPAR_DIP_SWITCHES_4BITS_DEVICE_ID);
}

void prvSetupSoftwareInterrupt(void)
{
	// Hook up interrupt service routine
	xPortInstallInterruptHandler(XPAR_MICROBLAZE_0_INTC_DIP_SWITCHES_4BITS_IP2INTC_IRPT_INTR, vSoftwareInterruptHandler, NULL);

	// Enable the interrupt vector at the interrupt controller
	vPortEnableInterrupt(XPAR_MICROBLAZE_0_INTC_DIP_SWITCHES_4BITS_IP2INTC_IRPT_INTR);

	// Enable the GPIO channel interrupts so that dip switch can be
	// detected and enable interrupts for the GPIO device.
	XGpio_InterruptEnable(&switch_Gpio, SWITCH_CHANNEL);
	XGpio_InterruptGlobalEnable(&switch_Gpio);
}

int main(void)
{
	// Configure both the hardware and the debug interface.
	vSetupEnvironment();

	// Before a queue can be used it must first be created. Create both queues
	// used by this example. One queue can hold variables of type unsigned long,
	// the other queue can hold variables of type char*. Both queues can hold a
	// maximum of 10 items. A real application should check the return values to
	// ensure the queues have been successfully created.
	xIntegerQueue = xQueueCreate(10, sizeof(unsigned long));
	xStringQueue = xQueueCreate(10, sizeof(char*));


	// Enable the software interrupt and set its priority.
	prvSetupSoftwareInterrupt();

	// Create the task that uses a queue to pass integers to the interrupt
	// service routine. The task is created a priority 1.
	xTaskCreate(vIntegerGenerator, "IntGen", configMINIMAL_STACK_SIZE,
					NULL, tskIDLE_PRIORITY+1, NULL);

	// Create the task that prints out the strings sent to it from the interrupt
	// service routine. This task is created at the higher priority of 2.
	xTaskCreate(vStringPrinter, "String", configMINIMAL_STACK_SIZE,
				NULL, tskIDLE_PRIORITY+2, NULL);

	// Start the scheduler so the created tasks start executing.
	vTaskStartScheduler();

	// If all is well we will never reach here as the scheduler will now be
	// running the tasks. If we do reach here then it is likely that there was
	// insufficient heap memory available for a resource to be created.

	for (;;);

	return 0;
}

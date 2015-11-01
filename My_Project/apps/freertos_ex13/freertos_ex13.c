#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "xil_printf.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xgpio.h"

#define SWITCH_CHANNEL	XGPIO_IR_CH1_MASK

xSemaphoreHandle xCountingSemaphore;
XGpio switch_Gpio;

static void vPeriodicTask(void* pvParameters)
{
	// As per most tasks, this implemented within an infinite loop.
	for (;;)
	{
		// This task is just used to 'simulate' an interrupt. This is done by
		// periodically generating a software interrupt.
		vTaskDelay(500/portTICK_RATE_MS);

		// Generating the interrupt, printing a message both before hand and
		// afterwards so the sequence of execution is evident from the output.
		vTaskSuspendAll();
		xil_printf("Periodic task - About to generate an interrupt.\n");
		xTaskResumeAll();
		XGpio_Out32(switch_Gpio.BaseAddress+XGPIO_ISR_OFFSET, XGPIO_IR_CH1_MASK);
		vTaskSuspendAll();
		xil_printf("Periodic task - Interrupt generated.\n\n");
		xTaskResumeAll();
	}
}

static void vHandlerTask(void* pvParameters)
{
	// As per most tasks, this task is implemented within an infinite loop.

	// Take the semaphore once to start with so the semaphore is empty before
	// the infinite loop is entered. The semaphore was created before the
	// scheduler was started so before this task ran for the first time.
	xSemaphoreTake(xCountingSemaphore, 0);
	for (;;)
	{
		// Use the semaphore to wait for the event. The task blocks indefinitely
		// meaing this function call will only return once the semaphore has
		// been successfully obtained - so there is no need to check the
		// returned value.
		xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);

		// To get here the event must have occurred. Process the event (in this
		// case we just print out a message).
		vTaskSuspendAll();
		xil_printf("Handler task - Processing event.\n");
		xTaskResumeAll();
	}
}

void vSoftwareInterruptHandler()
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	// 'Give" the semaphore multiple times. The first will unblock the handler
	// task, the following 'gives' are to demonstrate that the semaphore latches
	// the events to allow the handler task to process them in turn without any
	// events getting lost. This simulate multiple interrupts being taken by
	// the processor, even though in this case the events are simulated within
	// a single interrupt occurance.
	xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);

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

	// Before a semaphore is used it must be explicitly created. In this example
	// a counting semaphore is created. The semaphore is created to have a
	// maximum count value of 10, and an initial count value of 0.
	xCountingSemaphore = xSemaphoreCreateCounting(10, 0);

	// Check the semaphore was created successfully.
	if (xCountingSemaphore != NULL)
	{
		// Enable the software interrupt and set its priority.
		prvSetupSoftwareInterrupt();

		// Create the 'handler' task. This is the task that will be synchronized
		// with the interrupt. The handler task is created with a high priority
		// to ensure it runs immediately after the interrupt exits. In this
		// case a priority of 3 is chosen.
		xTaskCreate(vHandlerTask, "Handler", configMINIMAL_STACK_SIZE, NULL,
		tskIDLE_PRIORITY+3, NULL);

		// Create the task that will periodically generate a software interrupt.
		// This is created with a priority below the handler task to ensure it
		// will get preempted each time the handler task exits the Blocked state

		xTaskCreate(vPeriodicTask, "Periodic", configMINIMAL_STACK_SIZE, NULL,
		tskIDLE_PRIORITY+1, NULL);

		// Start the scheduler so the created tasks start executing.
		vTaskStartScheduler();
	}

	// If all is well we will never reach here as the scheduler will now be
	// running the tasks. If we do reach here then it is likely that there was
	// insufficient heap memory available for a resource to be created.

	for (;;);

	return 0;
}

#include "FreeRTOS.h"
#include "task.h"

#include "xil_printf.h"
#include "xil_cache.h"

void vTaskFunction(void* pvParameters)
{
	char* pcTaskName;

	// The string to print out is passed in via the parameter. Cast this to a
	// character pointer.
	pcTaskName = (char*)pvParameters;

	// As per most tasks, this task is implemented in an infinite loop.
	for (;;)
	{
		// Print out the name of this task.
		vTaskSuspendAll();
		xil_printf(pcTaskName);
		xTaskResumeAll();

		// Delay for a period. This time a call to vTaskDelay() is used which
		// places the task into the Blocked state until the delay period has
		// expired. The delay period is specified in 'ticks', but the constant
		// portTICK_RATE_MS can be used to convert this to a more user friendly
		// value in milliseconds. In this case a period of 250 milliseconds is
		// being specified.
		vTaskDelay(250/portTICK_RATE_MS);
	}
}


// Define the strings that will be passed in as the task parameters. These are
// defined const and not on the stack to ensure they remain valid when the
// tasks are executing.
static const char* pcTextForTask1 = "Task 1 is running\n";
static const char* pcTextForTask2 = "Task 2 is running\n";

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

	// Create the first task at priority 1. The priority is the second to 
	// last parameter.
	xTaskCreate( vTaskFunction, "Task 1", configMINIMAL_STACK_SIZE, 
		 (void*)pcTextForTask1, tskIDLE_PRIORITY+1, NULL);

	// Create the second task at priority 2.
	xTaskCreate(vTaskFunction, "Task 2", configMINIMAL_STACK_SIZE,
		(void*)pcTextForTask2, tskIDLE_PRIORITY+2, NULL);

	// Start the scheduler so our tasks start executing.
	vTaskStartScheduler();

	// If all is well then main() will never reach here as the scheduler will
	// now be running the tasks. If main() does reach here then it is likely
	// that there was insufficient heap memory available for the idle task to
	// be created. Chapter 5 provides more information on memory management.
	for (;;);

	return 0;
}

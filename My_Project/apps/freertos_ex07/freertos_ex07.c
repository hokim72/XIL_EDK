#include "FreeRTOS.h"
#include "task.h"

#include "xil_printf.h"

// Declare a variable that will be incremented by the hook function.
unsigned long ulIdleCycleCount = 0UL;

// Idle hook functions MUST be called vApplicationIdleHook(), task no parameters, and return void.
void vApplicationIdleHook(void)
{
	// This hook function does nothing but increment a counter.
	ulIdleCycleCount++;
}

void vTaskFunction(void* pvParameters)
{
	char* pcTaskName;

	// The string to print out is passed in via the parameter. Cast this to a
	// character pointer.
	pcTaskName = (char*)pvParameters;

	// As per most tasks, this task is implemented in an infinite loop.
	for (;;)
	{
		// Print out the name of this task AND the number of times ulIdleCycleCount has been incremented.
		vTaskSuspendAll();
		xil_printf("%s, ulIdleCycleCount = %d\n", pcTaskName, ulIdleCycleCount);
		xTaskResumeAll();

		// Delay for a period of 250 milliseconds.
		vTaskDelay(250/portTICK_RATE_MS);
	}
}

// defined const and not on the stack to ensure they remain valid when the tasks
// are executing.
static const char* pcTextForTask1 = "Task 1 is running";
static const char* pcTextForTask2 = "Task 2 is running";

int main(void)
{
	// Create the first task at priority 1. The priority is the second to last
	// parameter.
	xTaskCreate(vTaskFunction, "Task 1", configMINIMAL_STACK_SIZE,
				(void*)pcTextForTask1, tskIDLE_PRIORITY+1, NULL);

	// Create the second task at priority 2.
	xTaskCreate(vTaskFunction, "Task 2", configMINIMAL_STACK_SIZE,
				(void*)pcTextForTask2, tskIDLE_PRIORITY+2, NULL);

	// Start the scheduler so our tasks start executing.
	vTaskStartScheduler();

	// If all is well then main() will never reach here as the scheduler will
	// now be running the tasks. If main() does reach here then it is likely
	// that there was insufficient heap memory available for the idle task
	// to be created.
	// Chapter 5 provides more information on memory management.
	for (;;);

	return 0;
}

#include "FreeRTOS.h"
#include "task.h"

#include "xil_printf.h"

// Declare a variable that is used to hold the handle of Task2.
xTaskHandle xTask2Handle;

void vTask2(void* pvParameters)
{
	// Task 2 does nothing but delete itself. To do this it could call
	// vTaskDelete() usign NULL as the parameter, but instead and purely for
	// demonstration purposes it instead calls vTaskDelete() passing its own
	// task handle.
	vTaskSuspendAll();
	xil_printf("Task2 is running and about to delete itself\n");
	xTaskResumeAll();
	vTaskDelete(xTask2Handle);
}

void vTask1(void* pvParameters)
{
	const portTickType xDelay100ms = 100 / portTICK_RATE_MS;

	for (;;)
	{
		// Print out the name of this task.
		vTaskSuspendAll();
		xil_printf("Task 1 is running\n");
		xTaskResumeAll();

		// Create task2 at a higher priority. Again the task parameter is not
		// used so is set to NULL - BUT this time the task handle is required so
		// the address of xTask2Handle is passed as the last parameter.
		xTaskCreate(vTask2, "Task 2", configMINIMAL_STACK_SIZE, NULL,
										tskIDLE_PRIORITY+2, &xTask2Handle);
		// The task handle is the last parameter____________^^^^^^^^^^^^^

		// Task 2 has/had the higher priority, so for Task1 to reach here Task 2
		// must have already executed and deleted itself. Delay for 100
		// milliseconds.
		vTaskDelay(xDelay100ms);
	}
}

int main(void)
{
	// Create the first task at priority 1. The task parameter is not used
	// so is set to NULL. The task handle is also not used so likewise is set
	// to NULL.
	xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE, NULL,
											tskIDLE_PRIORITY+1, NULL);
	// The task is created at priority 1 ____________________^.

	// Start the scheduler so the task starts executing.
	vTaskStartScheduler();

	// main() should never reach here as the scheduler has been started.
	for (;;);

	return 0;
}

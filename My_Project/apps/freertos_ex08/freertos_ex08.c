#include "FreeRTOS.h"
#include "task.h"

#include "xil_printf.h"
#include "xil_cache.h"

// Declare a variable that is used to hold the handle of Task 2.
xTaskHandle xTask2Handle;

void vTask1(void* pvParameters)
{
	unsigned portBASE_TYPE uxPriority;

	// This task will always run before Task 2 as it is created with the higher
	// priority. Neither Task 1 nor Task 2 ever block so both will always be in
	// either the Running or the Ready state.

	// Query the priority at which this task is running - passing in NULL means
	// "return my priority".
	uxPriority = uxTaskPriorityGet(NULL);

	for (;;)
	{
		// Print out the name of this task.
		vTaskSuspendAll();
		xil_printf("Task 1 is running\n");
		xTaskResumeAll();

		// Setting the Task 2 priority above the Task 1 priority will cause
		// Task 2 to immediately start running (as then Task 2 will have the
		// higher of the two created tasks). Note the use of the handle to task
		// 2 (xTask2Handle) in the call to vTaskPrioritySet(). main() shows how
		// the handle was obtained.
		vTaskSuspendAll();
		xil_printf("About to raise the Task 2 priority\n");
		xTaskResumeAll();
		vTaskPrioritySet(xTask2Handle, uxPriority + 1);

		// Task 1 will only run when it has a priority higher than Task 2.
		// Therefore, for this task to reach this point Task 2 must already have
		// executed and set its priority back down to below the priority of this
		// task.
	}
}

void vTask2(void* pvParameters)
{
	unsigned portBASE_TYPE uxPriority;

	// Task 1 will always run before this task as Task 1 is created with the
	// higher priority. Neither Task 1 nor Task 2 ever block so will always be
	// in either the Running or the Ready state.

	// Query the priority at which this task is running - passing in NULL means
	// "return my priority".
	uxPriority = uxTaskPriorityGet(NULL);

	for (;;)
	{
		// For this task to reach this point Task 1 must have already run and
		// set the priority of this task higher than its own.

		// Print out the name of this task.
		vTaskSuspendAll();
		xil_printf("Task2 is running\n");
		xTaskResumeAll();

		// Set our priority back down to its original value. Passing in NULL
		// as the task handle means "change my priority". Setting the priority
		// below that of Task 1 will cause Task 1 to immediately start
		// running again - pre-empting this task.
		vTaskSuspendAll();
		xil_printf("About to lower the Task 2 priority\n");
		xTaskResumeAll();
		vTaskPrioritySet(NULL, uxPriority - 2);
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

	// Create the first task at priority 2. The task parameter is not used
	// and set to NULL. The task handle is also not used so is also set to NULL.
	xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE, NULL, 
							tskIDLE_PRIORITY+2, NULL);
	// The task is created at priority 2-----^

	// Create the second task at priority 1 - which is lower than the priority
	// given to Task 1. Again the task parameter is not used so is set to NULL -
	// BUT this time the task handle is required so the address of xTask2Handle
	// is passed int the last parameter.
	xTaskCreate(vTask2, "Task 2", configMINIMAL_STACK_SIZE, NULL,
							tskIDLE_PRIORITY+1, &xTask2Handle);
	// The task handle is the last parameter ___^^^^^^^^^^^^^

	// Start the scheduler so the tasks start executing.
	vTaskStartScheduler();

	// If all is well the main() will never reach here as the scheduler will
	// now be running the tasks. If main() does reach here then it is likely
	// that there was insufficient heap memory available for the idle task to
	// be created. Chapter 5 provides more information on memory management.
	for (;;);

	return 0;
}


#include "FreeRTOS.h"
#include "task.h"

#include "xil_printf.h"
#include "xil_cache.h"

#define mainDELAY_LOOP_COUNT 100000

void vTask1(void* pvParameters)
{
	const char* pcTaskName = "Task 1 is running\n";
	volatile unsigned long ul;

	// As per most tasks, this task is implemented in an infinite loop.
	for (;;)
	{
		// Print out the name of this task.
		vTaskSuspendAll();
		xil_printf(pcTaskName);
		xTaskResumeAll();

		// Delay for a period.
		for (ul=0; ul<mainDELAY_LOOP_COUNT; ul++)
		{
			// This loop is just a very crude delay implementation. There is
			// nothing to do in here. Later examples will replace this crude
			// loop with a proper delay/sleep function.
		}
	}
}

void vTask2(void* pvParameters)
{
	const char* pcTaskName = "Task 2 is running\n";
	volatile unsigned long ul;

	// As per most tasks, this task is implemented in an infinite loop.
	for (;;)
	{
		// Print out the name of this task.
		vTaskSuspendAll();
		xil_printf(pcTaskName);
		xTaskResumeAll();

		// Delay for a period.
		for (ul=0; ul<mainDELAY_LOOP_COUNT; ul++)
		{
			// This loop is just a very crude delay implementation. There is
			// nothing to do in here. Later examples will replace this crude
			// loop with a proper delay/sleep function.
		}
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
	
	// Create one of the two tasks. Note that a real application should check
	// the return value of the xTaskCreate() call to ensure the task was created
	// successfully.
	xTaskCreate( vTask1, // Pointer to the function that implements the task.
				 "Task1", // Text name for the task. This is to facilitate
				 		  // debugging only.
				 configMINIMAL_STACK_SIZE, // Stack depth in words.
				 NULL, // We are not using the task parameter.
				 tskIDLE_PRIORITY+1, // This task will run at priority 1.
				 NULL); // We are not going to use the task handle.
	
	// Create the other task in exactly the same way and at the same priority.
	xTaskCreate(vTask2, "Task2", configMINIMAL_STACK_SIZE, NULL,
				tskIDLE_PRIORITY+1, NULL);
	
	// Start the scheduler so the tasks start executing.
	vTaskStartScheduler();

	// If all is well then main() will never reach here as the scheduler will
	// now be running the tasks. If main() does reach here then it is likely
	// that there was insufficient heap memory available for the idle task
	// to be created.
	// Chapeter 5 provides more information on memory management.
	for (;;);

	return 0;
}

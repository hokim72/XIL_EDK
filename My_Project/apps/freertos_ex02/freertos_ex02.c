#include "FreeRTOS.h"
#include "task.h"

#include "xil_printf.h"
#include "xil_cache.h"

#define mainDELAY_LOOP_COUNT 100000

void vTaskFunction(void* pvParameters)
{
	char* pcTaskName;
	volatile unsigned long ul;

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

		// Delay for a period.
		for (ul=0; ul<mainDELAY_LOOP_COUNT; ul++)
		{
			// This loop is just a very crude delay implementation. There is
			// nothing to do in here. Later exercises will replace this crude
			// loop with a proper delay/sleep function.
		}
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

	// Create one of the two tasks.
	xTaskCreate( vTaskFunction, // Pointer to the function that implements
								// the task.
				 "Task 1", // Text name for the task. This is to facilitate
				 		   // debugging only.
				 configMINIMAL_STACK_SIZE, // Stack depth in words.
				 (void*)pcTextForTask1, // Pass the text to be printed into the
				 						// task using the task parameter.
				 tskIDLE_PRIORITY+1, // This task will run at priority 1.
				 NULL);

	// Create the other task in exactly the same way. Note this time that
	// multiple tasks are being created from the SAME task implementation 
	// (vTaskFunction). Only the value passed in the parameter is different.
	// Two instance of the same task are being created.
	xTaskCreate(vTaskFunction, "Task 2", configMINIMAL_STACK_SIZE,
		(void*)pcTextForTask2, tskIDLE_PRIORITY+1, NULL);

	// Start the scheduler so our tasks start executing.
	vTaskStartScheduler();

	// If all is well then main() will never reach here as the scheduler will
	// now be running the tasks. If main() does reach here then it is likely
	// that there was insufficient heap memory available for the idle task to
	// be created. Chapter 5 provides more information on memory management.
	for (;;);

	return 0;
}

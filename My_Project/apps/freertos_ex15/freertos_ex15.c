#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "xil_printf.h"
#include "xil_cache.h"

xSemaphoreHandle xMutex;

static void prvNewPrintString(const char* pcString)
{
	// The mutex is created before the scheduler is started so already
	// exists by the time this task first executes.

	// Attempt to take the mutex, blocking indefinitely to wait for the mutex
	// if it is not available straight away. The call to xSemaphoreTake() will
	// only return when the mutex has been successfully obtained so there is
	// no need to check the function return value. If any other delay period
	// was used then the code must check that xSemaphoreTake() returns pdTRUE
	// before accessing the shared resource (which in this case is standard out).
	xSemaphoreTake(xMutex, portMAX_DELAY);

	// The following line will only execute once the mutex has been successfully
	// obtained. Standard out can be accessed freely now as only one task
	// can have the mutex at any one time.
	xil_printf(pcString);

	// The mutex MUST be given back!
	xSemaphoreGive(xMutex);
}

static void prvPrintTask(void* pvParameters)
{
	char* pcStringToPrint;

	// Two instances of this task are created so the string the task will send
	// to prvNewPrintString() is passed into the task using the task parameter.
	// Cast this to the required type.
	pcStringToPrint = (char*) pvParameters;

	for (;;)
	{
		// Print out the string using the newly defined function.
		prvNewPrintString(pcStringToPrint);

		// Wait a pseudo random time. Note that rand() is not necessarily
		// reentrant, but in this case it does not really matter as the code
		// does not care what value is returned. In a more secure application
		// a version of rand() that is known to be reentrant should be used
		// - or calls to rand() should be protected using a critical section.
		vTaskDelay(rand() & 0x1FF);
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

	// Before a semaphore is used it must be explicitly created. In this example
	// a mutex type semaphore is created.
	xMutex = xSemaphoreCreateMutex();

	// The tasks are going to use a pseudo random delay, seed the random number
	// generator.
	srand(567);

	// Only create the tasks if the semaphore was created successfully.
	if (xMutex != NULL)
	{
		// Create two instances of the tasks that write to stdout. The string
		// they write is passed in as the task parameter. The tasks are created
		// at different priorities so some pre-emption will occur.
		xTaskCreate(prvPrintTask, "Print1", configMINIMAL_STACK_SIZE,
		"Task 1 *********************************\n", tskIDLE_PRIORITY+1, NULL);

		xTaskCreate(prvPrintTask, "Print2", configMINIMAL_STACK_SIZE,
		"Task 2 ---------------------------------\n", tskIDLE_PRIORITY+1, NULL);

		// Start the scheduler so the created tasks start executing.
		vTaskStartScheduler();
	}

	// If all is well then main() will never reach here as the scheduler will
	// now be running the tasks. If main() does reach here then it is likely
	// that there was insufficient heap memory available for the idle task to be
	// created. Chapter 5 provides more information on memory management.
	for (;;);

	return 0;
}

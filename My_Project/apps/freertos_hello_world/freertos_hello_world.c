#include "FreeRTOS.h"
#include "task.h"

#include "xil_printf.h"
#include "xil_cache.h"

#define mainHELLO_WORLD_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define	mainGOOD_BYE_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

static void prvHelloWorld(void* pvParameters);
static void prvGoodBye(void* pvParameters);

int main( void )
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

	// Start the two tasks
	xTaskCreate(prvHelloWorld, "HW", configMINIMAL_STACK_SIZE, NULL,
				mainHELLO_WORLD_TASK_PRIORITY, NULL);
	xTaskCreate(prvGoodBye, "GB", configMINIMAL_STACK_SIZE, NULL,
				mainGOOD_BYE_TASK_PRIORITY, NULL);

	// Start the tasks and timer running
	vTaskStartScheduler();

	// If all is well, the scheduler will now be running, and the following line
	// will never be reached. If the following line does execute, then there was
	// insufficient FreeRTOS heap memory available for the idle and/or timer
	// tasks to be created. See the memory management section on the FreeRTOS
	// web site for more details.
	for (;;);
	
	return 0;
}

static void prvHelloWorld(void* pvParameters)
{
	for (;;)
	{
		xil_printf("Hello World\r\n");
		taskYIELD();
	}
}

static void prvGoodBye(void* pvParameters)
{
	for (;;)
	{
		xil_printf("Good Bye\r\n");
		taskYIELD();
	}
}

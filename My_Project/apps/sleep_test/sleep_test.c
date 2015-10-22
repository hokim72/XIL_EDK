#include "xil_cache.h"
#include "xil_printf.h"
#include "sleep.h"

int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	int count = 0;
	while(1)
	{
		sleep(1);
		xil_printf("count = %d\n", count++);
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}

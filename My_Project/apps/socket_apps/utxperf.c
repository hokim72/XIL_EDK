#include <stdio.h>
#include <string.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"

#ifdef __arm__
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#endif

void print_utxperf_app_header()
{
        xil_printf("%20s %6s %s\r\n", "txperf client",
                        "N/A",
                        "$ iperf -s -u -i 5 (on host with IP 192.168.1.100)");
}

#define SEND_BUFSIZE (1400)
static char send_buf[SEND_BUFSIZE];

void utxperf_application_thread()
{
	struct ip_addr ipaddr;
	int i, sock;
	struct sockaddr_in serv_addr;
	int id = 0;
	int *p = (int *)(&(send_buf[0]));

	IP4_ADDR(&ipaddr,  192, 168,   1, 100);		/* iperf server address */

	/* initialize data buffer being sent */
	for (i = 0; i < SEND_BUFSIZE; i++)
		send_buf[i] = (i % 10) + '0';

	if ((sock = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		xil_printf("error creating socket\r\n");
#ifdef OS_IS_FREERTOS
		vTaskDelete(NULL);
#endif
		return;
	}

	memset((void*)&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5001);
	serv_addr.sin_addr.s_addr = ipaddr.addr;

	print("Connecting to iperf UDP server...");
	if (lwip_connect(sock, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
		xil_printf("error in connect\r\n");
		return;
	}
	print("Connected\r\n");

	while (1) {
		id++;
		if (id == 0x7FFFFFFE)
			id = 0;
		p[0] = htonl(id);

		if ((lwip_write(sock, (const void*)send_buf, 1400)) < 0)
			break;
	}

	print("TX perf stopped\r\n");
#ifdef OS_IS_FREERTOS
	vTaskDelete(NULL);
#endif
	return;
}

#include <stdio.h>
#include <string.h>
#include "lwipopts.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

static unsigned rxperf_port = 5001;	/* iperf default port */

void print_rxperf_app_header()
{
        xil_printf("%20s %6d %s\r\n", "rxperf server",
                        rxperf_port,
                        "$ iperf -c <board ip> -i 5 -t 100");
}

void rx_application_thread()
{
	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(rxperf_port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0) {
#ifdef OS_IS_FREERTOS
		vTaskDelete(NULL);
#endif
		return;
	}

	lwip_listen(sock, 0);

	size = sizeof(remote);
	new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t*)&size);

	while (1) {
#if (USE_JUMBO_FRAMES==1)
		char recv_buf[9700];
		/* keep reading data */
		if (lwip_read(new_sd, recv_buf, 8000) <= 0)
			break;
#else
		char recv_buf[1500];
		/* keep reading data */
		if (lwip_read(new_sd, recv_buf, 1400) <= 0)
			break;
#endif
	}

	print("Connection closed. RXPERF exiting.\r\n");

    lwip_close(new_sd);
#ifdef OS_IS_FREERTOS
	xil_printf("Rx IPERF Thread is being DELETED\r\n");
    vTaskDelete(NULL);
#endif
}
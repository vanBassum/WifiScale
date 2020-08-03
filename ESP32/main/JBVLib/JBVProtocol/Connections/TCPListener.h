/*
 * TCPListener.h
 *
 *  Created on: 1 Aug 2020
 *      Author: Bas
 */

#ifndef MAIN_JBVLIB_ETHERNET_TCPLISTENER_H_
#define MAIN_JBVLIB_ETHERNET_TCPLISTENER_H_
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "esp_log.h"
#include "stdint.h"
#include "../../FreeRTOS/TaskCPP.h"
#include "../../Callback.h"
#include "TCPConnection.h"

class TCPListener
{
	Task<void> *task = NULL;
	uint16_t port;

public:

	Callback<void, TCPConnection*> *ConnectionRecieved = NULL;

private:


	void Task_DoWork()
	{
		char addr_str[128];
		int ip_protocol = 0;
		struct sockaddr_in6 dest_addr;

		struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
		dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
		dest_addr_ip4->sin_family = AF_INET;
		dest_addr_ip4->sin_port = htons(port);
		ip_protocol = IPPROTO_IP;

		int listen_sock = socket(AF_INET, SOCK_STREAM, ip_protocol);
		if (listen_sock < 0)
		{
			ESP_LOGE("TCPListener", "Unable to create socket: errno %d", errno);
			vTaskDelete(NULL);
			return;
		}

		ESP_LOGI("TCPListener", "Socket created");

		int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
		if (err != 0)
		{
			ESP_LOGE("TCPListener", "Socket unable to bind: errno %d", errno);
			ESP_LOGE("TCPListener", "IPPROTO: %d", AF_INET);
			goto CLEAN_UP;
		}

		ESP_LOGI("TCPListener", "Socket bound, port %d", port);

		err = listen(listen_sock, 1);
		if (err != 0)
		{
			ESP_LOGE("TCPListener", "Error occurred during listen: errno %d", errno);
			goto CLEAN_UP;
		}

		while (1)
		{

			ESP_LOGI("TCPListener", "Socket listening");

			struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
			uint addr_len = sizeof(source_addr);
			int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
			if (sock < 0)
			{
				ESP_LOGE("TCPListener", "Unable to accept connection: errno %d", errno);
				break;
			}

			// Convert ip address to string
			if (source_addr.sin6_family == PF_INET) {
				inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
			} else if (source_addr.sin6_family == PF_INET6) {
				inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
			}
			ESP_LOGI("TCPListener", "Socket accepted ip address: %s", addr_str);

			TCPConnection *con = new TCPConnection(sock);
			if(ConnectionRecieved != NULL)
				(*ConnectionRecieved)(con);

		}

	CLEAN_UP:
		close(listen_sock);
		vTaskDelete(NULL);

	}

public:

	TCPListener(uint16_t port)
	{
		this->port = port;
		task = new Task<void>("TCPListener", 5, 1024 * 4, this, &TCPListener::Task_DoWork);
		task->Run();
	}


	~TCPListener()
	{
		delete task;
	}


};




#endif /* MAIN_JBVLIB_ETHERNET_TCPLISTENER_H_ */

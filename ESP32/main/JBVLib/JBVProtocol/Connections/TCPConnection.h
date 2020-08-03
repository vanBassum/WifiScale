/*
 * TCPConnection.h
 *
 *  Created on: 1 Aug 2020
 *      Author: Bas
 */

#ifndef COMPONENTS_JBVPROTOCOL_CONNECTIONS_TCPCONNECTION_H_
#define COMPONENTS_JBVPROTOCOL_CONNECTIONS_TCPCONNECTION_H_

#include "freertos/FreeRTOS.h"
#include "ESP_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "../../FreeRTOS/TaskCPP.h"
#include "../IO/Connection.h"

class TCPConnection : public Connection
{
	int sock = NULL;
	Task<void> *task = NULL;

	char *_host_ip = NULL;
	uint16_t _port = 0;
	uint32_t _retryInterval = 0;

	void TryConnect()
	{
		int addr_family = 0;
		int ip_protocol = 0;
		struct sockaddr_in dest_addr;
		dest_addr.sin_addr.s_addr = inet_addr(_host_ip);
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(_port);
		addr_family = AF_INET;
		ip_protocol = IPPROTO_IP;

		if(sock != NULL)
		{
			//Cleanup the previous socket.
	        shutdown(sock, 0);
	        close(sock);
	        sock = NULL;
		}

		sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
		if (sock < 0)
		{
			ESP_LOGE("TCPConnection", "Unable to create socket: errno %d", errno);
			sock = 0;
			return;
		}
		ESP_LOGI("TCPConnection", "Socket created, connecting to %s:%d", _host_ip, _port);

		int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
		if (err != 0)
		{
			ESP_LOGE("TCPConnection", "Socket unable to connect: errno %d", errno);
			shutdown(sock, 0);
			close(sock);
			sock = 0;
			return;
		}
		ESP_LOGI("TCPConnection", "Successfully connected");


	}


	void Task_DoWork()
	{
	    int len;
	    uint8_t rx_buffer[128];


	    while(_host_ip != NULL || sock != NULL)
	    {
	    	while(sock!=NULL)
	    	{
	    		len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
				if (len < 0)
				{
					ESP_LOGE("TCPConnection.h", "Error occurred during receiving: errno %d", errno);
				}
				else if (len == 0)
				{
					ESP_LOGW("TCPConnection.h", "Connection closed");
				}
				else
				{
					HandleData(rx_buffer, len);
				}

				if(len < 0)
				{
					shutdown(sock, 0);
					close(sock);
					sock = 0;
				}
	    	}


	    	//Not connected, try to connect.

	    	if(_host_ip != NULL)
	    	{
	    		TryConnect();
	    		if(sock == NULL)
	    			vTaskDelay(_retryInterval / portTICK_PERIOD_MS);
	    	}
	    }


	    ESP_LOGI("TCPConnection.h", "Closing connection");
		if(OnDisconnected != NULL)
			(*OnDisconnected)(this);
	}

public:

	TCPConnection(const char *host_ip, uint16_t port, uint32_t retryInterval)
	{
		uint8_t len = strlen(host_ip)+1;
		_host_ip = (char *) malloc(len);
		memcpy(_host_ip, host_ip, len);
		_port = port;
		_retryInterval = retryInterval;

		task = new Task<void>("TCPConnection.h", 5, 1024 * 4, this, &TCPConnection::Task_DoWork);
		task->Run();
	}


	TCPConnection(int socket)
	{
		sock = socket;
		task = new Task<void>("TCPConnection.h", 5, 1024 * 4, this, &TCPConnection::Task_DoWork);
		task->Run();
	}

	~TCPConnection()
	{
		ESP_LOGI("TCPConnection.h", "Destructor");
		if(_host_ip != NULL)
			free(_host_ip);
		delete task;
        shutdown(sock, 0);
        close(sock);
	}

	bool SendData(uint8_t *data, uint8_t length)
	{
        int to_write = length;
        while (to_write > 0)
        {
            int written = send(sock, data + (length - to_write), to_write, 0);
            if (written < 0)
            {
                ESP_LOGE("TCPConnection.h", "Error occurred during sending: errno %d", errno);
                return false;
            }
            to_write -= written;
        }
        return true;
	}

};












#endif /* COMPONENTS_JBVPROTOCOL_CONNECTIONS_TCPCONNECTION_H_ */

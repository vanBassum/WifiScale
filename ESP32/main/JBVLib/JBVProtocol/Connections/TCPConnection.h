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
	int sock = 0;
	Task<void> *task = NULL;

	void Task_DoWork()
	{
	    int len;
	    uint8_t rx_buffer[128];

	    do
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
	    }
	    while (len > 0);

	    ESP_LOGI("TCPConnection.h", "Closing connection");
        shutdown(sock, 0);
        close(sock);
		if(OnDisconnected != NULL)
			(*OnDisconnected)(this);
	}

public:



	TCPConnection(int socket)
	{
		sock = socket;
		task = new Task<void>("TCPConnection.h", 5, 1024 * 4, this, &TCPConnection::Task_DoWork);
		task->Run();
	}

	~TCPConnection()
	{
		ESP_LOGI("TCPConnection.h", "Destructor");
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

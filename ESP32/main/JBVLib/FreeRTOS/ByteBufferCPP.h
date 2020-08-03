/*
 * MessageBufferCPP.h
 *
 *  Created on: Dec 6, 2019
 *      Author: Bas
 */

#ifndef MAIN_SMARTHOME_LIB_FREERTOS_MESSAGEBUFFERCPP_H_
#define MAIN_SMARTHOME_LIB_FREERTOS_MESSAGEBUFFERCPP_H_

#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"
#include <vector>
#include <stdio.h>
#include <string.h>

class ByteBuffer
{
	RingbufHandle_t buf_handle = NULL;

public:


	/*void operator +=(uint8_t b)
	{
		Enqueue(&b, 1, 0);
	}*/

	ByteBuffer(size_t size)
	{
		buf_handle = xRingbufferCreate(size, RINGBUF_TYPE_BYTEBUF);
	}

	~ByteBuffer()
	{
		vRingbufferDelete(buf_handle);
	}


	bool EnqueueByte(uint8_t data, int length, int timeout)
	{
		return xRingbufferSend(buf_handle, &data, length, timeout / portTICK_PERIOD_MS) == pdTRUE;
	}

	bool Enqueue(uint8_t *data, int length, int timeout)
	{
		return xRingbufferSend(buf_handle, data, length, timeout / portTICK_PERIOD_MS) == pdTRUE;
	}

	uint8_t* Dequeue(size_t *item_size, size_t maxLength, int timeout)
	{
		return (uint8_t *)xRingbufferReceiveUpTo(buf_handle, item_size, timeout/portTICK_PERIOD_MS, maxLength);
	}

	void DisposeItem(uint8_t *item)
	{
		vRingbufferReturnItem(buf_handle, (void *)item);
	}

	size_t GetFreeSize()
	{
		return xRingbufferGetCurFreeSize(buf_handle);
	}




	/*bool Enqueue(std::vector<uint8_t> data, int timeout)
	{
		return xRingbufferSend(buf_handle, &data[0], data.size(), timeout / portTICK_PERIOD_MS) == pdTRUE;
	}*/

};



#endif /* MAIN_SMARTHOME_LIB_FREERTOS_MESSAGEBUFFERCPP_H_ */

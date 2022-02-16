#pragma once
#include "lib/tcpip/tcpsocket.h"
#include "lib/misc/datetime.h"
#include "lib/rtos/queue.h"
#include "esp_timer.h"
#include "sample.h"
#include <cjson.h>

FreeRTOS::NotifyableTask sendTask;
FreeRTOS::Queue<Sample> sendQueue(10);

void InitSendSamples()
{
	sendTask.SetCallback(SendTaskHandler);
	sendTask.Run("Send task", 7, 2048);
}


void SendTaskHandler(FreeRTOS::NotifyableTask* task, void* args)
{
	TCPSocket socket;
	DateTime validTime = DateTime("2022-01-01T12:00:00");

	//Wait until the time is OKE.
	while (DateTime::Now() < validTime)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}


	while (true)
	{
		Sample* sample = NULL;
		while (sendQueue.Dequeue(sample, 10000))	//Wait until there is a sample to transmit
		{
			if (sample->Timestamp < validTime)
			{
				//Correction for measurements taken before valid time
				DateTime now = DateTime::Now();
				uint64_t secondsSinceStart = esp_timer_get_time() / 1000000;
				uint32_t seconds = now.utc - secondsSinceStart + sample->Timestamp.utc;
				sample->Timestamp = DateTime(seconds);
			}

			if (socket.Connect("192.168.11.50", 1000, false, 5000))
			{
				//Convert to JSON
				cJSON* command = cJSON_CreateObject();
				cJSON_AddNumberToObject(command, "CMD", 0x01);
				cJSON* data = cJSON_CreateObject();
				cJSON_AddNumberToObject(command, "RawWeight", sample->Raw);
				cJSON_AddNumberToObject(command, "Weight", sample->Weight);
				cJSON_AddStringToObject(command, "MeasurementTimestamp", sample->Timestamp.ToString().c_str());
				cJSON_AddItemToObject(command, "Data", data);

				//Send the sample
				char* json = cJSON_PrintUnformatted(command);
				int len = strlen(json);
				socket.Send((uint8_t*)json, len);
				cJSON_free(command);
				free(json);
				delete sample;
				sample = NULL;
			}
			else
			{
				//If it wasn't send, send back to queue.
				sendQueue.Enqueue(sample, 1000);
			}
		}
		socket.Disconnect();
	}
}

void EnqueueSample(Sample sample)
{
	Sample* copy = new Sample();
	memcpy(copy, &sample, sizeof(Sample));
	sendQueue.Enqueue(copy, 1000);
}
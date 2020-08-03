/*
 * Semaphore.h
 *
 *  Created on: Dec 3, 2019
 *      Author: Bas
 */

#ifndef MAIN_SMARTHOME_LIB_FREERTOS_SEMAPHORECPP_H_
#define MAIN_SMARTHOME_LIB_FREERTOS_SEMAPHORECPP_H_

#define ProtectedOrReboot(sem, timeout, MSG) \
    for(int i=0; i<2; i++)\
        if(i==1)\
            sem.Give();\
        else\
			if(sem.TakeOrReboot(timeout, MSG))


#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "QueueSet.h"

extern "C"
{
	#include "../../../source/log/log.h"
}





class SemaphoreBase
{
protected:
	SemaphoreHandle_t handle = NULL;
	//friend QueueSet;
public:

	bool EnableLog = false;

	/// <summary>
	/// Deletes the semaphore, including mutex type semaphores and recursive semaphores.
	/// Do not delete a semaphore that has tasks blocked on it (tasks that are in the Blocked state waiting for the semaphore to become available).
	/// </summary>
	virtual ~SemaphoreBase()
	{
		vSemaphoreDelete(handle);
		handle = NULL;
	}

	/// <summary>
	/// Deletes the semaphore, including mutex type semaphores and recursive semaphores.
	/// </summary>
	/// <param name="timeout">Timeout in milisecond.</param>
	/// <returns>TRUE if the semaphore was obtained. FALSE if timeout expired without the semaphore becoming available.</returns>
	bool Take(int timeout)
	{
		if(EnableLog)
			ESP_LOGI("SEM", "Take %p", handle);
		return xSemaphoreTake(handle, timeout / portTICK_PERIOD_MS) == pdTRUE;
	}


	log_data_t prevTakeLog;

	bool TakeOrReboot(int timeout, const char* msg)
	{


		bool oke = xSemaphoreTake(handle, timeout / portTICK_PERIOD_MS) == pdTRUE;

		if(oke)
		{
			Log_ResetLogData(&prevTakeLog, LOG_CODE_MUTEX_NOT_TAKEN_PREV);
			snprintf((char*)prevTakeLog.data, LOG_DATA_SIZE, msg);
		}
		else
		{
			Log_AddLog(&prevTakeLog, 1);	//Add previous take to log
			log_data_t log_temp;
			Log_ResetLogData(&log_temp, LOG_CODE_MUTEX_NOT_TAKEN);
			snprintf((char*)log_temp.data, LOG_DATA_SIZE, msg);
			Log_AddLog(&log_temp,1);
			esp_restart();
		}

		return oke;
	}



	void TakeForever()
	{
		xSemaphoreTake(handle, portMAX_DELAY);
	}


	bool TakeFromISR(int *higherPriorityTaskWoken = NULL)
	{
		return xSemaphoreTakeFromISR(handle, higherPriorityTaskWoken) == pdTRUE;
	}


	bool Give()
	{
		if(EnableLog)
			ESP_LOGI("SEM", "Give %p", handle);
		return xSemaphoreGive(handle) == pdTRUE;
	}

	bool GiveFromISR(int *higherPriorityTaskWoken = NULL)
	{
		return xSemaphoreGiveFromISR(handle, higherPriorityTaskWoken) == pdTRUE;
	}

	int Count()
	{
		return uxSemaphoreGetCount(handle);
	}

};


///
/// <summary>
/// Use a mutex or recursive mutex to protect a resource, use semaphore for task synchronizing
/// https://www.freertos.org/Real-time-embedded-RTOS-mutexes.html </summary>
///
///
class Mutex : public SemaphoreBase
{
public:
	Mutex()
	{
		handle = xSemaphoreCreateMutex();
	}

};

/// <summary>
/// Use a mutex or recursive mutex to protect a resource, use semaphore for task synchronizing
/// https://www.freertos.org/Real-time-embedded-RTOS-mutexes.html </summary>
///
class RecursiveMutex
{
	SemaphoreHandle_t handle = NULL;
public:
	RecursiveMutex()
	{
		handle = xSemaphoreCreateRecursiveMutex();
	}

	bool Take(int timeout)
	{
		return xSemaphoreTakeRecursive(handle, timeout / portTICK_PERIOD_MS) == pdTRUE;
	}

	log_data_t prevTakeLog;
	bool TakeOrReboot(int timeout, const char* msg)
	{
		bool oke = xSemaphoreTakeRecursive(handle, timeout / portTICK_PERIOD_MS) == pdTRUE;
		if(oke)
		{
			Log_ResetLogData(&prevTakeLog, LOG_CODE_MUTEX_NOT_TAKEN_PREV);
			snprintf((char*)prevTakeLog.data, LOG_DATA_SIZE, msg);
		}
		else
		{
			Log_AddLog(&prevTakeLog, 1);	//Add previous take to log
			log_data_t log_temp;
			Log_ResetLogData(&log_temp, LOG_CODE_MUTEX_NOT_TAKEN);
			snprintf((char*)log_temp.data, LOG_DATA_SIZE, msg);
			Log_AddLog(&log_temp,1);
			esp_restart();
		}

		return oke;
	}

	void Give()
	{
		xSemaphoreGiveRecursive(handle);
	}

};

///
/// <summary> Use a mutex or recursive mutex to protect a resource, use semaphore for task synchronizing
/// https://www.freertos.org/Embedded-RTOS-Binary-Semaphores.html </summary>
///
class SemaphoreBinary : public SemaphoreBase, public QueueSetAddable
{
public:
	SemaphoreBinary(bool free = false)
	{
		handle = xSemaphoreCreateBinary();
		if(free)
			Give();
	}

	int GetSize() {return 1;}
	QueueSetMemberHandle_t GetHandle() {return (QueueSetMemberHandle_t)handle;}
};


///
/// <summary> Use a mutex or recursive mutex to protect a resource, use semaphore for task synchronizing
/// https://www.freertos.org/Real-time-embedded-RTOS-Counting-Semaphores.html </summary>
///
class SemaphoreCounting : public SemaphoreBase, public QueueSetAddable
{
	int maxCnt;
public:
	SemaphoreCounting(int maxCount, int initialCount)
	{
		maxCnt = maxCount;
		handle = xSemaphoreCreateCounting(maxCount, initialCount);
	}

	int GetSize() {return maxCnt;}
	QueueSetMemberHandle_t GetHandle() {return (QueueSetMemberHandle_t)handle;}
};

/*
class RecursiveMutex : public SemaphoreBase
{
public:
	RecursiveMutex()
	{
		//Not yet supported!
	}
};
*/




#endif /* MAIN_SMARTHOME_LIB_FREERTOS_SEMAPHORECPP_H_ */

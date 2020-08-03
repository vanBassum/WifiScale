/*
 * TaskV2.h
 *
 *  Created on: Dec 13, 2019
 *      Author: Bas
 */

#ifndef MAIN_SMARTHOME_LIB_FREERTOS_INVOKER_H_
#define MAIN_SMARTHOME_LIB_FREERTOS_INVOKER_H_

#include "taskcpp.h"
#include "esp_log.h"
#include "../Callback.h"
#include "queuecpp.h"
#include <tuple>

/*
class InvokeItem
{
	Callback<void, void*> _callback;
	void *_arg;

public:

	template<typename T>
	InvokeItem(void *arg, T *instance, void (T::*workCallback)(Args...)) : _callback(instance, workCallback)
	{
		_arg = arg;
	}

	InvokeItem(void *arg, void (*workCallback)(Args...)) : _callback(workCallback)
	{
		_arg = arg;
	}

	void Invoke()
	{
		_callback(arg);
	}
};




class Invoker : private Task<>
{



private:
	Queue<InvokeItem> _invokeQueue = Queue<InvokeItem>(10);

	void Work()
	{
		while(true)
		{
			InvokeItem cb;

			if(_invokeQueue.Dequeue(cb, 24*60*60))
			{
				cb.Invoke();
			}
		}
	}


public:

	Invoker(const char *name, portBASE_TYPE prio, portSHORT stackDepth=configMINIMAL_STACK_SIZE) : Task<>(name, prio, stackDepth)
	{
		Run();
	}

	~Invoker()
	{

	}

	void Invoke(void (*workCallback)(void*), int timeout, void* arg = NULL)
	{
		InvokeItem itm = InvokeItem(arg, workCallback);
		_invokeQueue.Enqueue(itm);
	}

	template<typename T>
	void Invoke(T *instance, void (T::*workCallback)(void*), int timeout, void* arg = NULL)
	{
		InvokeItem itm = InvokeItem(arg, instance, workCallback);
		_invokeQueue.Enqueue(itm);
	}
};











/*



/// <summary>
/// Wrapper to create a freertos tasks in C++.
/// To use, create a new class that inherits this class and override the Work method.
/// <example>
/// <code>
/// class SomeClass : public Task
/// {
/// private:
///		void Work(): Task("SomeClass", 7, 2048)
///		{
///			printf("Hello");
///			vTaskDelay(1000 / portTICK_PERIOD_MS);
/// 	}
/// public:
///		SomeClass()
/// 	{
///			//Do the initializer before calling Run() !!!
///			Run();
/// 	}
/// };
/// </code>
/// </example>
/// </summary>
template<typename T = void> class Task;


//Per default, dont use the notification system.
template<>
class Task<void> : public TaskImpl
{
public:
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth=configMINIMAL_STACK_SIZE) : TaskImpl(name, prio, stackDepth){}
};



class NotifyBinary;
/// <summary>
/// Implementation of task where the notify system is used as Binary Semaphore.
/// </summary>
template<> class Task<NotifyBinary> : public TaskImpl
{
	class NotifyBinary
	{
		Task& task;

	protected:

		bool Wait(int timeout)
		{
			return ulTaskNotifyTake(true, timeout / portTICK_PERIOD_MS) == pdPASS;
		}

	public:
		NotifyBinary(Task& parent) : task(parent)
		{

		}

		void Give()
		{
			xTaskNotifyGive(task.taskHandle);
		}

		void GiveFromISR(BaseType_t *pxHigherPriorityTaskWoken = NULL)
		{
			vTaskNotifyGiveFromISR(task.taskHandle, pxHigherPriorityTaskWoken);
		}
	};

public:
	NotifyBinary notify;
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth=configMINIMAL_STACK_SIZE) : TaskImpl(name, prio, stackDepth), notify(*this){}
};

class NotifyCounting;
/// <summary>
/// Implementation of task where the notify system is used as Binary Semaphore.
/// </summary>
template<> class Task<NotifyCounting> : public TaskImpl
{
	class NotifyCounting
	{
		Task& task;

	protected:

		bool Wait(int timeout)
		{
			return ulTaskNotifyTake(false, timeout / portTICK_PERIOD_MS) == pdPASS;
		}

	public:
		NotifyCounting(Task& parent) : task(parent)
		{

		}

		void Give()
		{
			xTaskNotifyGive(task.taskHandle);
		}

		void GiveFromISR(BaseType_t *pxHigherPriorityTaskWoken = NULL)
		{
			vTaskNotifyGiveFromISR(task.taskHandle, pxHigherPriorityTaskWoken);
		}
	};

public:
	NotifyCounting notify;
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth=configMINIMAL_STACK_SIZE) : TaskImpl(name, prio, stackDepth), notify(*this){}
};



class NotifyEventGroup;
/// <summary>
/// Implementation of task where the notify system is used as Binary Semaphore.
/// </summary>
template<> class Task<NotifyEventGroup> : public TaskImpl
{
	class NotifyEventGroup
	{
		Task& task;

	protected:

		bool Wait(uint32_t *pulNotificationValue, int timeout)
		{
			return xTaskNotifyWait( 0x0000, 0xFFFF, pulNotificationValue, timeout / portTICK_PERIOD_MS) == pdPASS;
		}

	public:
		NotifyEventGroup(Task& parent) : task(parent)
		{

		}

		void Set(uint32_t bits)
		{
			xTaskNotify(task.taskHandle, bits, eSetBits);
		}

		void SetFromISR(uint32_t bits, BaseType_t *pxHigherPriorityTaskWoken = NULL)
		{
			xTaskNotifyFromISR(task.taskHandle, bits, eSetBits, pxHigherPriorityTaskWoken);
		}

	};

public:
	NotifyEventGroup notify;
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth=configMINIMAL_STACK_SIZE) : TaskImpl(name, prio, stackDepth), notify(*this){}
};


*/





#endif

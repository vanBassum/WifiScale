/*
 * TaskCPP.h
 *
 *  Created on: Nov 8, 2019
 *      Author: Bas
 */
//Took some advise from:
//https://www.freertos.org/FreeRTOS_Support_Forum_Archive/July_2010/freertos_Is_it_possible_create_freertos_task_in_c_3778071.html

#ifndef _TaskCPP_H_
#define _TaskCPP_H_


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "../Callback.h"

#define ISSET(events, ev)	(events & (uint32_t)ev)

/// <summary>
/// Helperclass for Task<>
/// Don't use this directly
/// </summary>
class TaskBase
{

private:
	char const *_name;
	portBASE_TYPE _priority;
	portSHORT _stackDepth;
	Callback<void> *work = NULL;
	xTaskHandle taskHandle;


	void WorkInt()
	{
		if(work != NULL)
			(*work)();
	}

	static void taskfun(void* parm)
	{
		TaskBase* t = static_cast<TaskBase*>(parm);
		t->WorkInt();
		t->taskHandle = NULL;
		vTaskDelete(NULL);
	}

public:

	TaskBase(const char *name, portBASE_TYPE priority, portSHORT stackDepth, void (*fp)())
	{
		if(stackDepth < configMINIMAL_STACK_SIZE)
			stackDepth = configMINIMAL_STACK_SIZE;

		_name = name;
		_priority = priority;
		_stackDepth = stackDepth;
		taskHandle = NULL;
		work = new Callback<void>(fp);
		//Don't create the task yet. To prevent the task beeing started before the constructor is finished.
		//xTaskCreate(&taskfun, name, stackDepth, this, priority, &handle);
	}

	template<typename T>
	TaskBase(const char *name, portBASE_TYPE priority, portSHORT stackDepth, T *instance, void (T::*mp)())
	{
		if(stackDepth < configMINIMAL_STACK_SIZE)
			stackDepth = configMINIMAL_STACK_SIZE;

		_name = name;
		_priority = priority;
		_stackDepth = stackDepth;
		taskHandle = NULL;
		work = new Callback<void>(instance, mp);
		//Don't create the task yet. To prevent the task beeing started before the constructor is finished.
		//xTaskCreate(&taskfun, name, stackDepth, this, priority, &handle);
	}

	/// <summary>
	/// Deconstructor.
	/// </summary>
	~TaskBase()
	{
		//ESP_LOGI("TASKCPP","Destructor handle '%d'", (int)&taskHandle);
		if(taskHandle != NULL)
			vTaskDelete(taskHandle);

		delete work;
		//ESP_LOGI("TASKCPP","Destructed");
		return;
	}

	/// <summary>
	/// Used to start the task.
	/// This should only be called after the initializer is done.
	/// Otherwise the task could run before the initializer is finished resulting in possible hardfaults.
	/// </summary>
	void Run()
	{
		xTaskCreate(&taskfun, _name, _stackDepth, this, _priority, &taskHandle);
	}

};



template<typename T = void> class Task;


//Per default, don't use the notification system.
template<>
class Task<void> : public TaskBase
{
public:
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth, void (*fp)()) 					: TaskBase(name, prio, stackDepth, fp){}
	template<typename T>
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth, T *instance, void (T::*mp)()) 	: TaskBase(name, prio, stackDepth, instance, mp){}

};

/*

class NotifyBinary
{
	TaskBase& task;

public:

	bool Wait(int timeout)
	{
		return ulTaskNotifyTake(true, timeout / portTICK_PERIOD_MS) == pdPASS;
	}


	NotifyBinary(TaskBase& parent) : task(parent)
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
/// <summary>
/// Implementation of task where the notify system is used as Binary Semaphore.
/// </summary>
template<> class Task<NotifyBinary> : public TaskBase
{
public:

	NotifyBinary notify;
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth=configMINIMAL_STACK_SIZE) : TaskBase(name, prio, stackDepth), notify(*this){}
};


class NotifyCounting
{
	TaskBase& task;

public:

	bool Wait(int timeout)
	{
		return ulTaskNotifyTake(false, timeout / portTICK_PERIOD_MS) == pdPASS;
	}


	NotifyCounting(TaskBase& parent) : task(parent)
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

/// <summary>
/// Implementation of task where the notify system is used as Binary Semaphore.
/// </summary>
template<> class Task<NotifyCounting> : public TaskBase
{
public:

	NotifyCounting notify;
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth, void (*fp)()) 					: TaskBase(name, prio, stackDepth, fp), notify(this){}
	template<typename T>
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth, T *instance, void (T::*mp)()) 	: TaskBase(name, prio, stackDepth, instance, mp), notify(this){}
};




class NotifyEventGroup
{
	TaskBase* task;

public:

	bool Wait(uint32_t *pulNotificationValue, int timeout)
	{
		return xTaskNotifyWait( 0x0000, 0xFFFF, pulNotificationValue, timeout / portTICK_PERIOD_MS) == pdPASS;
	}


	NotifyEventGroup(TaskBase* parent)
	{
		task = parent;
	}

	void Set(uint32_t bits)
	{
		xTaskNotify(task->taskHandle, bits, eSetBits);
	}

	void SetFromISR(uint32_t bits, BaseType_t *pxHigherPriorityTaskWoken = NULL)
	{
		xTaskNotifyFromISR(task->taskHandle, bits, eSetBits, pxHigherPriorityTaskWoken);
	}

};

/// <summary>
/// Implementation of task where the notify system is used as Binary Semaphore.
/// </summary>
template<> class Task<NotifyEventGroup> : public TaskBase
{
public:
	NotifyEventGroup notify;
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth, void (*fp)()) 					: TaskBase(name, prio, stackDepth, fp), notify(this){}
	template<typename T>
	Task(const char *name, portBASE_TYPE prio, portSHORT stackDepth, T *instance, void (T::*mp)()) 	: TaskBase(name, prio, stackDepth, instance, mp), notify(this){}

};

*/






#endif

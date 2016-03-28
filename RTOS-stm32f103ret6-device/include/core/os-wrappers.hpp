/*
 * sheduler.hpp
 *
 * This file contains C++ wrappers for FreeRTOS
 *
 *  Created on: 18 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_CORE_SHEDULER_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_CORE_SHEDULER_HPP_

#include "utils/macro.hpp"
#include "utils/interfaces.hpp"
#include "cmsis_os.h"
#include "hal/system-clock.hpp"
#include <functional>
#include <vector>
#include <list>

using STask = std::function<void(void)>;
using STime = uint32_t;
using STaskId = osThreadId;

class Kernel
{
public:
	static void yeld();

	void run();
	inline bool isRunning() const { return m_isRunning; }
	int getTasksCount();

	void assert(bool shouldBeTrue, const char* message);

	SIGLETON_IN_CLASS(Kernel);

private:
	Kernel() {}
	bool m_isRunning = false;
};

class TaskBase
{
public:
	TaskBase(const STask& task = nullptr, const char* name = "Unknown") :
		m_task(task),
		m_name(const_cast<char*>(name))
	{ }

	inline bool isRunning() const { return m_state & runningNow; }
	inline void deleteAfterRun(bool doDelete = true)
	{
		if (doDelete)
			m_state |= needDeleteSelf;
		else
			m_state &= ~(needDeleteSelf);
	}
	inline void setTask(const STask& _task) { m_task = _task; }
	inline void setStackSize(uint32_t stackSize) { m_stackSize = stackSize; }
	void setName(const char* name) { m_name = const_cast<char*>(name); }
	inline STaskId taskId() { return m_taskId; }

	/**
	 * Stop task safely, all destructors will be called
	 * @return is task executing after this call
	 */
	bool safelyStop();
	/**
	 * Call vTaskDelete. Attention! This operation immediately stop the task
	 * without calling any destructors
	 */
	void stopUnsafe();

protected:
	constexpr static uint8_t runningNow = 1;
	constexpr static uint8_t needDeleteSelf = 2;
	constexpr static uint8_t needStop = 4;
	constexpr static uint8_t executingNow = 8;

	void stopThread();

	STask m_task;
	uint32_t m_stackSize = 128;
	STaskId m_taskId = nullptr;
	uint8_t m_state = 0;
	char* m_name;
};

class TaskOnce : public TaskBase
{
public:
	TaskOnce(const STask& task = nullptr) :
		TaskBase(task)
	{ }
	bool run(STime delay = 0, osPriority priority = osPriorityNormal);

private:
	STime m_firstRunDelay = 0;
	static void runTaskOnce(void const* pTask);
};

class TaskCycled : public TaskBase
{
public:
	TaskCycled(const STask& task = nullptr) :
			TaskBase(task)
	{ }

	bool run(STime firstRunDelay = 0, STime periodMin = 1, STime periodMax = 1, uint32_t cyclesCount = 0);

private:
	STime m_firstRunDelay = 0;
	STime m_periodMin = 0;
	STime m_periodMax = 0;
	uint32_t m_cyclesCount = 0;
	static void runTaskInCycle(void const* pTask);
};

class TaskDeferredFromISR
{
public:
	/**
	 * Check if task is already waiting to run
	 * @return true if is waiting
	 */
	bool isPlanned();
	/**
	* Defer task runing from ISR using Free RTOS mechanism.
	* Calls yeld so would stop the ISR
	* @param task
	* @return false if task is already planned
	*/
	bool run(STask&& task);

private:
	static void taskBody(void* arg, uint32_t argSize);
	STask m_task = nullptr;
};

class Mutex
{
public:
	Mutex()		{ handle = xSemaphoreCreateMutex(); }
	~Mutex() 	{ if (handle) vSemaphoreDelete(handle); }

	inline bool lock(uint32_t timeout = portMAX_DELAY)
		{ return (xSemaphoreTake( handle, timeout ) == pdTRUE); }

	inline void unlock()
		{ xSemaphoreGive( handle ); }

	inline bool isLocked()
	{
		bool wasNotLocked = lock(0);
		if (wasNotLocked) {
			unlock();
			return false;
		}
		return true;
	}
private:
	xSemaphoreHandle handle = nullptr;
};

class CritialSection
{
public:
	void lock() { taskENTER_CRITICAL(); m_isLocked = true; }
	void unlock() { taskEXIT_CRITICAL(); m_isLocked = false; }
	bool isLocked() { return m_isLocked; }

private:
	bool m_isLocked = false;
};

template<typename Lockable>
class ScopedLock
{
public:
	ScopedLock(Lockable& mutex) :
		m_mutex(mutex)
	{
		m_mutex.lock();
	}

	~ScopedLock()
	{
		unlock();
	}

	inline void unlock()
	{
		if (m_mutex.isLocked())
		m_mutex.unlock();
	}

private:
	Lockable& m_mutex;
};

template<typename T>
class Queue
{
public:
	Queue(unsigned int size = 10)
	{
		m_handle = xQueueCreate(size, sizeof(T));
	}

	void pushBack(T&& obj, TickType_t timeToWait = portMAX_DELAY)
	{
		xQueueSendToBack(m_handle, &obj, timeToWait);
	}

	void pushFront(T&& obj, TickType_t timeToWait = portMAX_DELAY)
	{
		xQueueSendToFront(m_handle, &obj, timeToWait);
	}

	void popFront(T& target, TickType_t timeToWait = portMAX_DELAY)
	{
		xQueueReceive(m_handle, &target, timeToWait);
	}

	void pushBackFromISR(T&& obj)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendToBackFromISR(m_handle, &obj, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
	}

	void pushFrontFromISR(T&& obj)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendToFrontFromISR(m_handle, &obj, xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
	}

	void popFrontFromISR(T& target)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueReceiveFromISR(m_handle, &target, xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
	}

	uint16_t size()
	{
		return uxQueueMessagesWaiting(m_handle);
	}

private:
	xQueueHandle m_handle;
};


class Interrogator
{
public:
	Interrogator();
	void registerObject(IInterrogatable* object);
	void run(uint32_t period = 1);
	void setStackSize(uint32_t stackSize);
	void setName(const char* name);

private:
	void interrogateAll();
	TaskCycled m_task;
	std::vector<IInterrogatable*> m_objects;
};

/// Tasks pool with cooperative multitasking. One class instance = one OS thread
class TasksPool
{
public:
	using TaskId = uint16_t;
	TasksPool(const char *name = "UnkPool");
	TaskId add(STask&& newTask, uint32_t period, uint32_t firstDelay = 0, uint32_t count = 0, uint32_t lifetime = 0);
	TaskId addOnce(STask&& newTask, uint32_t firstDelay);
	void stop(TaskId id);
	void setStackSize(uint16_t stackSize);
	void setName(char *name);
	void run(uint32_t sleepTime = 1);

private:
	void main();

	struct TaskContext
	{
		TaskId taskId = 0;
		STask task = nullptr;
		Time timeToRun = 0;
		Time timeout = 0;
		uint32_t period = 100000;
		uint32_t repetitionsCount = 0;
		uint32_t repetitionsCountMax = 0;
	};

	std::list<TaskContext> m_runningTasks;
	/// One mutex for both add and delete task lists, because actual adding and deleting operations are quick
	Mutex m_taskModifyMutex;
	std::list<TaskContext> m_tasksToAdd;
	std::list<TaskId> m_idsToDelete;
	TaskId m_nextTaskId = 1;

	TaskCycled m_poolThread;
	char* m_name;
};

class Worker
{
public:
	using WTask = std::function<void(void)>;

	Worker(uint8_t maxQueueSize);

	void setStackSize(uint32_t stackSize);
	void run();
	inline void add(WTask task)
	{
		m_queue.pushBack(std::forward<WTask>(task));
	}

	inline void addFromISR(WTask task)
	{
		m_queue.pushBackFromISR(std::forward<WTask>(task));
	}
	void setName(const char* name) { m_workerThread.setName(name); }

private:
	void mainLoop();

	TaskOnce m_workerThread;
	Queue<WTask> m_queue;
	WTask m_nextTask = nullptr;
};

#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_CORE_SHEDULER_HPP_ */

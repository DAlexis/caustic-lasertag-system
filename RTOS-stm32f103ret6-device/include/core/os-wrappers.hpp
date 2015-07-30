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
#include "cmsis_os.h"
#include <functional>
#include <vector>

using STask = std::function<void(void)>;
using STime = uint32_t;
using STaskId = osThreadId;

class Kernel
{
public:
	Kernel() {}

	static void yeld();

	void run();
	inline bool isRunning() const { return m_isRunning; }
	SIGLETON_IN_CLASS(Kernel);

private:
	bool m_isRunning = false;
};

class TaskBase
{
public:
	TaskBase(const STask& task = nullptr) :
		m_task(task)
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
};

class TaskOnce : public TaskBase
{
public:
	TaskOnce(const STask& task = nullptr) :
		TaskBase(task)
	{ }
	bool run(STime delay = 0);

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

	bool run(STime firstRunDelay = 0, STime periodMin = 0, STime periodMax = 0, uint32_t cyclesCount = 0);

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

class ScopedLock
{
public:
	ScopedLock(Mutex& mutex) :
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
	Mutex& m_mutex;
};

class IInterrogatable
{
public:
	virtual ~IInterrogatable() {}
	virtual void interrogate() = 0;
};

class Interrogator
{
public:
	Interrogator();
	void registerObject(IInterrogatable* object);
	void run(uint32_t period = 0);
	void setStackSize(uint32_t stackSize);

private:
	void interrogateAll();
	TaskCycled m_task;
	std::vector<IInterrogatable*> m_objects;
};
#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_CORE_SHEDULER_HPP_ */

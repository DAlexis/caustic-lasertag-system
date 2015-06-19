/*
 * sheduler.hpp
 *
 *  Created on: 18 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_CORE_SHEDULER_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_CORE_SHEDULER_HPP_

#include "utils/macro.hpp"
#include "cmsis_os.h"
#include <functional>

using STask = std::function<void(void)>;
using STime = uint32_t;
using STaskId = osThreadId;

class TaskBase
{
public:
	TaskBase(const STask& _task = nullptr) :
		task(_task)
	{ }
	inline bool isRunning() const { return state & runningNow; }
	inline void deleteAfterRun(bool doDelete = true)
	{
		if (doDelete)
			state |= needDeleteSelf;
		else
			state &= ~(needDeleteSelf);
	}

	/**
	 * Stop task safely, all destructors will be called
	 * @return is stopped immediately or executing now and can not be stopped
	 */
	bool safelyStop();
	constexpr static uint8_t runningNow = 1;
	constexpr static uint8_t needDeleteSelf = 2;
	constexpr static uint8_t needStop = 4;
	constexpr static uint8_t executingNow = 8;
	STask task;
	uint32_t stackSize = 128;
	STaskId taskId = nullptr;
	uint8_t state = 0;
};

class TaskOnce : public TaskBase
{
public:
	TaskOnce(const STask& task = nullptr) :
		TaskBase(task)
	{
	}
	bool run(STime delay);

	STime firstRunDelay = 0;
};

class TaskCycled : public TaskBase
{
public:
	TaskCycled(const STask& task = nullptr) :
			TaskBase(task)
	{
	}
	void run(STime _firstRunDelay, STime _periodMin, STime _periodMax);

	STime firstRunDelay = 0;
	STime periodMin = 0;
	STime periodMax = 0;
};

/*
class Scheduler
{
public:
	Scheduler();
	STaskId runOnce(const STask& task, STime delay, uint32_t stackSize = 256);
	STaskId runInCycle(const STask& task, STime firstRunDelay, STime periodMin, STime periodMax, uint32_t count = 0);
	SIGLETON_IN_CLASS(Scheduler);

	struct TaskRunOnceDescr
	{
		STask task;
		STime delay;
	};

	struct TaskRunInCycleDescr
	{
		STask task;
		STime firstRunDelay;
		STime periodMin;
		STime periodMax;
		uint32_t countTotal;
	};
private:
};
*/
#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_CORE_SHEDULER_HPP_ */

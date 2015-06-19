/*
 * scheduler.cpp
 *
 *  Created on: 18 июня 2015 г.
 *      Author: alexey
 */

#include "core/scheduler.hpp"
#include "core/logging.hpp"
#include "cmsis_os.h"

//SINGLETON_IN_CPP(Scheduler)

extern "C" void runTaskOnce(void const* pTask)
{
	TaskOnce *task = reinterpret_cast<TaskOnce*> (const_cast<void*>(pTask));
	osDelay(task->firstRunDelay);

	ADD_BITS(task->state, TaskBase::executingNow);
	task->task();

	if (task->state & TaskBase::needDeleteSelf)
		delete task;
	else
	{
		REMOVE_BITS(task->state, TaskBase::executingNow);
		REMOVE_BITS(task->state, TaskBase::runningNow);
	}
	vTaskDelete(NULL);
}
/*
extern "C" void runTaskInCycle(void const* pTaskDescr)
{
	const Scheduler::TaskRunInCycleDescr *pDescr = reinterpret_cast<const Scheduler::TaskRunInCycleDescr*> (pTaskDescr);
	osDelay(pDescr->firstRunDelay);
	if (pDescr->countTotal == 0)
	{
		for (;;)
		{
			pDescr->task();
			osDelay(pDescr->periodMin);
		}
	} else {
		for (;;)
		{
			pDescr->task();
			osDelay(pDescr->periodMin);
		}
	}
	delete pDescr;
	vTaskDelete(NULL);
}*/

bool TaskBase::safelyStop()
{
	if (!(state & runningNow))
		return true;

	if (state & executingNow)
	{
		ADD_BITS(state, needStop);
		return false;
	} else {
		vTaskDelete(taskId);
		taskId = nullptr;
		REMOVE_BITS(state, runningNow);
		return true;
	}
}

bool TaskOnce::run(STime delay)
{
	ADD_BITS(state, runningNow);
	firstRunDelay = delay;
	osThreadDef(newTask, runTaskOnce, osPriorityNormal, 0, stackSize);
	taskId = osThreadCreate(osThread(newTask), reinterpret_cast<void*>(this));
	if (taskId == NULL)
	{
		REMOVE_BITS(state, runningNow);
		return false;
	}
	return true;
}

/*
Scheduler::Scheduler()
{

}

STaskId Scheduler::runOnce(const STask& task, STime delay, uint32_t stackSize)
{
	TaskRunOnceDescr *descr = new TaskRunOnceDescr;
	descr->delay = delay;
	descr->task = task;
	osThreadDef(newTask, runTaskOnce, osPriorityNormal, 0, stackSize);
	return osThreadCreate(osThread(newTask), reinterpret_cast<void*>(descr));
}

STaskId Scheduler::runInCycle(const STask& task, STime firstRunDelay, STime periodMin, STime periodMax, uint32_t count)
{

}*/

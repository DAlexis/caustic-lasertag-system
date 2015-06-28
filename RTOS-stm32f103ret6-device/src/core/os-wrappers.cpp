/*
 * scheduler.cpp
 *
 *  Created on: 18 июня 2015 г.
 *      Author: alexey
 */

#include "core/os-wrappers.hpp"
#include "core/logging.hpp"
#include "cmsis_os.h"

//SINGLETON_IN_CPP(Scheduler)

void TaskOnce::runTaskOnce(void const* pTask)
{
	TaskOnce *task = reinterpret_cast<TaskOnce*> (const_cast<void*>(pTask));
	osDelay(task->m_firstRunDelay);

	ADD_BITS(task->m_state, executingNow);
	task->m_task();

	if (task->m_state & needDeleteSelf)
	{
		delete task;
		vTaskDelete(NULL);
	} else {
		REMOVE_BITS(task->m_state, executingNow);
		task->stopThread();
	}

}

void TaskCycled::runTaskInCycle(void const* pTask)
{
	TaskCycled *task = reinterpret_cast<TaskCycled*> (const_cast<void*>(pTask));
	osDelay(task->m_firstRunDelay);
	if (task->m_cyclesCount == 0)
	{
		for (;;)
		{
			ADD_BITS(task->m_state, executingNow);
			task->m_task();
			REMOVE_BITS(task->m_state, executingNow);
			if (task->m_state & needStop)
			{
				task->stopThread();
				return;
			}
			if (task->m_periodMin == 0)
				taskYIELD();
			else
				osDelay(task->m_periodMin);
		}
	} else {
		for (uint32_t i=0; i<task->m_cyclesCount;i++)
		{
			ADD_BITS(task->m_state, executingNow);
			task->m_task();
			REMOVE_BITS(task->m_state, executingNow);
			if (task->m_state & needStop)
			{
				task->stopThread();
				return;
			}
			if (task->m_periodMin == 0)
				taskYIELD();
			else
				osDelay(task->m_periodMin);
		}
		task->stopThread();
	}
}

bool TaskBase::safelyStop()
{
	if (!(m_state & runningNow))
		return true;

	if (m_state & executingNow)
	{
		ADD_BITS(m_state, needStop);
		return false;
	} else {
		vTaskDelete(m_taskId);
		m_taskId = nullptr;
		REMOVE_BITS(m_state, runningNow);
		REMOVE_BITS(m_state, needStop);
		return true;
	}
}

void TaskBase::stopUnsafe()
{
	if (m_taskId)
	{
		vTaskDelete(m_taskId);
		m_taskId = nullptr;
	}
}


void TaskBase::stopThread()
{
	REMOVE_BITS(m_state, runningNow);
	REMOVE_BITS(m_state, needStop);
	m_taskId = nullptr;
	vTaskDelete(NULL);
}

bool TaskOnce::run(STime delay)
{
	info << "Running new task, stack: " << (int)m_stackSize;
	ADD_BITS(m_state, runningNow);
	m_firstRunDelay = delay;
	osThreadDef(newTask, runTaskOnce, osPriorityNormal, 0, m_stackSize);
	m_taskId = osThreadCreate(osThread(newTask), reinterpret_cast<void*>(this));
	if (m_taskId == NULL)
	{
		REMOVE_BITS(m_state, runningNow);
		error << "Running new task failed!";
		return false;
	}
	info << "Running new task ok";
	return true;
}

bool TaskCycled::run(STime firstRunDelay, STime periodMin, STime periodMax, uint32_t cyclesCount)
{
	info << "Running new task, stack: " << (int)m_stackSize;
	ADD_BITS(m_state, runningNow);
	m_firstRunDelay = firstRunDelay;
	m_periodMin = periodMin;
	m_periodMax = periodMax;
	m_cyclesCount = cyclesCount;
	osThreadDef(newTask, runTaskInCycle, osPriorityNormal, 0, m_stackSize);
	m_taskId = osThreadCreate(osThread(newTask), reinterpret_cast<void*>(this));
	if (m_taskId == NULL)
	{
		REMOVE_BITS(m_state, runningNow);
		error << "Running new task failed!";
		return false;
	}
	info << "Running new task ok";
	return true;
}

bool TaskDeferredFromISR::isPlanned()
{
	return (m_task != nullptr);
}


bool TaskDeferredFromISR::run(STask&& task)
{
	if (m_task != nullptr)
		return false;

	m_task = task;


	/* The actual processing is to be deferred to a task.  Request the
	vProcessInterface() callback function is executed, passing in the
	number of the interface that needs processing.  The interface to
	service is passed in the second parameter.  The first parameter is
	not used in this case. */
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xTimerPendFunctionCallFromISR( &TaskDeferredFromISR::taskBody,
							   this,
							   sizeof(TaskDeferredFromISR),
							   &xHigherPriorityTaskWoken );

	/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
	switch should be requested.  The macro used is port specific and will
	be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
	the documentation page for the port being used. */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	return true;
}

void TaskDeferredFromISR::taskBody(void* arg, uint32_t argSize)
{
	UNUSED_ARG(argSize);
	TaskDeferredFromISR* object = reinterpret_cast<TaskDeferredFromISR*>(arg);
	//info << "Deferred task running";
	object->m_task();
	object->m_task = 0;
}

Interrogator::Interrogator()
{
	m_task.setTask(std::bind(&Interrogator::interrogateAll, this));
}

void Interrogator::registerObject(IInterrogatable* object)
{
	m_objects.push_back(object);
}

void Interrogator::run(uint32_t period)
{
	m_task.run(0, period, period, 0);
}

void Interrogator::setStackSize(uint32_t stackSize)
{
	m_task.setStackSize(stackSize);
}

void Interrogator::interrogateAll()
{
	for (auto it = m_objects.begin(); it != m_objects.end(); it++)
	{
		(*it)->interrogate();
	}
	taskYIELD();
}

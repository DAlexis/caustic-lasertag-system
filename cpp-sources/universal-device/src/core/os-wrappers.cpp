/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/


#include "core/os-wrappers.hpp"
#include "core/logging.hpp"
#include "cmsis_os.h"

SINGLETON_IN_CPP(Kernel)

void Kernel::run()
{
	info << "Starting FreeRTOS kernel";
	m_isRunning = true;
	osKernelStart();
}

void Kernel::yeld()
{
	if (Kernel::instance().m_isRunning)
		taskYIELD();
}

int Kernel::getTasksCount()
{
	return uxTaskGetNumberOfTasks();
}


void Kernel::assert(bool shouldBeTrue, const char* message)
{
	if (shouldBeTrue)
		return;

	error << "Fatal error: " << message;
	if (isRunning())
	{
		/// @todo Add here code to stop FreeRTOS
	}
	for (;;) ;
}

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

bool TaskOnce::run(STime delay, osPriority priority)
{
	info << "Running new task, stack: " << (int)m_stackSize;
	ADD_BITS(m_state, runningNow);
	m_firstRunDelay = delay;
	//osThreadDef(newTask, runTaskOnce, priority, 0, m_stackSize);

	os_thread_def threadDef;
	threadDef.name = m_name;
	threadDef.pthread = runTaskOnce;
	threadDef.tpriority = priority;
	threadDef.instances = 0;
	threadDef.stacksize = m_stackSize;

	m_taskId = osThreadCreate(&threadDef, reinterpret_cast<void*>(this));
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
	//osThreadDef(newTask, runTaskInCycle, osPriorityNormal, 0, m_stackSize);

	os_thread_def threadDef;
	threadDef.name = m_name;
	threadDef.pthread = runTaskInCycle;
	threadDef.tpriority = osPriorityNormal;
	threadDef.instances = 0;
	threadDef.stacksize = m_stackSize;

	m_taskId = osThreadCreate(&threadDef, reinterpret_cast<void*>(this));
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
	//info << "From ISR";

	UNUSED_ARG(argSize);
	TaskDeferredFromISR* object = reinterpret_cast<TaskDeferredFromISR*>(arg);
	//info << "Deferred task running";
	object->m_task();
	object->m_task = 0;
}

Interrogator::Interrogator(const char* name)
{
	m_task.setName(name);
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

void Interrogator::setName(const char* name)
{
	m_task.setName(name);
}


void Interrogator::interrogateAll()
{
	for (auto it = m_objects.begin(); it != m_objects.end(); it++)
	{
		(*it)->interrogate();
	}
	taskYIELD();
}

//////////////////////
// TasksPool
TasksPool::TasksPool(const char *name) :
	m_name(const_cast<char*>(name))
{
	m_poolThread.setTask([this](){ main(); });
}

TasksPool::TaskId TasksPool::add(STask&& newTask, uint32_t period, uint32_t firstDelay, uint32_t count, uint32_t lifetime)
{
	TaskContext context;
	Time time = systemClock->getTime();
	context.taskId = m_nextTaskId++;
	context.task = newTask;
	context.period = period;
	context.timeToRun = time + firstDelay;
	if (lifetime != 0)
	{
		context.timeout = time + lifetime;
	} else {
		context.timeout = 0;
	}

	context.repetitionsCountMax = count;

	ScopedLock<Mutex> lck(m_taskModifyMutex);
	m_tasksToAdd.push_back(context);
	return context.taskId;
}

TasksPool::TaskId TasksPool::addOnce(STask&& newTask, uint32_t firstDelay)
{
	return add(std::forward<STask>(newTask), 0, firstDelay, 1);
}


void TasksPool::setStackSize(uint16_t stackSize)
{
	m_poolThread.setStackSize(stackSize);
}

void TasksPool::setName(char *name)
{
	m_name = name;
}


void TasksPool::run(uint32_t sleepTime)
{
	m_poolThread.setName(m_name);
	m_poolThread.run(0, sleepTime, sleepTime);
}

void TasksPool::stop(TaskId id)
{
	ScopedLock<Mutex> lck(m_taskModifyMutex);
	m_idsToDelete.push_back(id);
}

void TasksPool::main()
{
	// Adding new tasks if exist
	m_taskModifyMutex.lock();
	for (auto it = m_tasksToAdd.begin(); it != m_tasksToAdd.end(); it = m_tasksToAdd.erase(it))
	{
		m_runningTasks.push_back(*it);
	}
	m_taskModifyMutex.unlock();

	// Running current tasks
	for (auto it = m_runningTasks.begin(); it != m_runningTasks.end(); )
	{
		Time time = systemClock->getTime();
		if (time >= it->timeToRun)
		{
			it->task();
			it->repetitionsCount++;
			it->timeToRun = time + it->period;
		}

		time = systemClock->getTime();
		if (
				(it->repetitionsCountMax != 0 && it->repetitionsCount == it->repetitionsCountMax)
				|| (it->timeout != 0 && time > it->timeout)
			)
		{
			it = m_runningTasks.erase(it);
		} else
			it++;
	}

	// Removing deleted tasks
	m_taskModifyMutex.lock();
	for (auto it = m_idsToDelete.begin(); it != m_idsToDelete.end(); it++)
	{
		for (auto jt = m_runningTasks.begin(); jt != m_runningTasks.end(); )
		{
			if (jt->taskId == *it)
			{
				jt = m_runningTasks.erase(jt);
			} else {
				jt++;
			}
		}
	}
	m_idsToDelete.clear();
	m_taskModifyMutex.unlock();
}

//////////////////////
// Worker
Worker::Worker(uint8_t maxQueueSize) :
	m_queue(maxQueueSize)
{
	m_workerThread.setTask([this](){ mainLoop(); });
}

void Worker::setStackSize(uint32_t stackSize)
{
	m_workerThread.setStackSize(stackSize);
}

void Worker::run()
{
	m_workerThread.run();
}

void Worker::mainLoop()
{
	for (;;)
	{
		m_queue.popFront(m_nextTask);
		if (m_nextTask == nullptr)
			break;
		else
			m_nextTask();
	}
}

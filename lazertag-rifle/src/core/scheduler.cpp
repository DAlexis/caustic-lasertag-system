/*
 * scheduler.cpp
 *
 *  Created on: 01 янв. 2015 г.
 *      Author: alexey
 */

#include "core/scheduler.hpp"
#include "hal/system-clock.hpp"

TaskId Scheduler::addTask(SchedulerTaskCallback task, bool runOnce, uint32_t period, uint32_t periodDelta, uint32_t firstTimeDelay)
{
	if (!task)
		return 0;

	m_tasksToAdd[m_lastEmptyId] = Task();
	m_tasksToAdd[m_lastEmptyId].callback = task;
	m_tasksToAdd[m_lastEmptyId].runOnce = runOnce;
	m_tasksToAdd[m_lastEmptyId].nextRun = systemClock->getTime() + firstTimeDelay;
	m_tasksToAdd[m_lastEmptyId].period = period;
	m_tasksToAdd[m_lastEmptyId].periodDelta = periodDelta;

	return m_lastEmptyId++;
}


void Scheduler::stopTask(TaskId taskId)
{
	m_toStop.push_back(taskId);
}

void Scheduler::stopTasksFromTemporaryList()
{
	for (auto it = m_toStop.begin(); it != m_toStop.end(); it++)
	{
		auto jt = m_tasks.find(*it);
		if (jt != m_tasks.end())
			m_tasks.erase(jt);
	}
}

void Scheduler::addTasksFromTemporaryList()
{
	m_tasks.insert(m_tasksToAdd.begin(), m_tasksToAdd.end());
	m_tasksToAdd.clear();
}

void Scheduler::setPeriod(TaskId taskId, uint32_t period, uint32_t periodDelta)
{
	auto it = m_tasks.find(taskId);
	if (it == m_tasks.end())
		return;
	it->second.period = period;
	it->second.periodDelta = periodDelta;
}

void Scheduler::mainLoop()
{
	for (;;)
	{
		uint32_t time = systemClock->getTime();
		for (auto it = m_tasks.begin(); it != m_tasks.end(); it++)
		{
			Task& task = it->second;
			if (task.nextRun <= time) {
				task.callback();
				task.nextRun = time + task.period;
				if (task.runOnce)
					stopTask(it->first);
			}
		}
		stopTasksFromTemporaryList();
		addTasksFromTemporaryList();
	}
}



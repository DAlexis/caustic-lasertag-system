/*
 * scheduler.hpp
 *
 *  Created on: 01 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_SCHEDULER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_SCHEDULER_HPP_

#include "core/singleton-macro.hpp"

#include <functional>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <stdint.h>

using SchedulerTaskCallback = std::function<void(void)>;


using TaskId = uint16_t;

class Scheduler
{
public:
	static Scheduler& instance();
	struct Task
	{
		SchedulerTaskCallback callback = nullptr;
		uint32_t nextRun = 0;
		uint32_t period = 0;
		uint32_t periodDelta = 0;
		bool runOnce = true;
	};

	TaskId addTask(SchedulerTaskCallback task, bool runOnce = true, uint32_t period = 0, uint32_t periodDelta = 0, uint32_t firstTimeDelay = 0);

	void stopTask(TaskId taskId);
	void setPeriod(TaskId taskId, uint32_t period, uint32_t periodDelta = 0);
	void mainLoop();

private:
	Scheduler() {}
	void stopTasksFromTemporaryList();
	void addTasksFromTemporaryList();

	TaskId m_lastEmptyId = 1;

	std::map<TaskId, Task> m_tasks;
	std::map<TaskId, Task> m_tasksToAdd;
	std::list<TaskId> m_toStop;

	static Scheduler* m_scheduler;
	STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;
};







#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_SCHEDULER_HPP_ */

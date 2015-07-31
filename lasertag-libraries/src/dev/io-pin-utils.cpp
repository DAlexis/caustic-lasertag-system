/*
 * io-pin-utils.cpp
 *
 *  Created on: 06 июня 2015 г.
 *      Author: alexey
 */

#include "dev/io-pin-utils.hpp"

#include "core/scheduler.hpp"

TaskId delayedSwitchPin(IIOPin* pin, bool state, uint32_t delay)
{
	if (true == state)
	{
		return Scheduler::instance().addTask(
				std::bind(&IIOPin::set, pin),
				true,
				0,
				0,
				delay
		);
	} else {
		return Scheduler::instance().addTask(
				std::bind(&IIOPin::reset, pin),
				true,
				0,
				0,
				delay
		);
	}
}

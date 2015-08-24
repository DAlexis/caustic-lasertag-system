/*
 * io-pins-utils.cpp
 *
 *  Created on: 24 авг. 2015 г.
 *      Author: alexey
 */

#include "dev/io-pins-utils.hpp"
#include "core/os-wrappers.hpp"

void delayedSwitchPin(IIOPin* pin, bool state, uint32_t delay)
{
	if (true == state)
		{
			DeferredTasksPool::instance().add(
					std::bind(&IIOPin::set, pin),
					delay
			);
		} else {
			DeferredTasksPool::instance().add(
					std::bind(&IIOPin::reset, pin),
					delay
			);
		}
}

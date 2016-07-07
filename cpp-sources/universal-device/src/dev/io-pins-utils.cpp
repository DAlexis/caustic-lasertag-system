/*
 * io-pins-utils.cpp
 *
 *  Created on: 24 авг. 2015 г.
 *      Author: alexey
 */

#include "dev/io-pins-utils.hpp"


void delayedSwitchPin(TasksPool &pool, IIOPin* pin, bool state, uint32_t delay)
{
	if (true == state)
		{
			pool.addOnce(
					std::bind(&IIOPin::set, pin),
					delay
			);
		} else {
			pool.addOnce(
					std::bind(&IIOPin::reset, pin),
					delay
			);
		}
}

void blinkPin(TasksPool &pool, IIOPin* pin, uint32_t delay)
{
	pin->set();
	delayedSwitchPin(pool, pin, false, delay);
}

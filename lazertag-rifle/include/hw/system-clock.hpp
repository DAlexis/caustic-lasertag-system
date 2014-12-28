/*
 * system-clock.hpp
 *
 *  Created on: 29 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_SYSTEM_CLOCK_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_SYSTEM_CLOCK_HPP_

#include "hal/system-clock.hpp"
#include <stdint.h>

class SystemClock : public ISystemClock
{
public:
	SystemClock();
	void wait_us(uint32_t time);
	uint32_t getTime();
};


#endif /* LAZERTAG_RIFLE_INCLUDE_HW_SYSTEM_CLOCK_HPP_ */

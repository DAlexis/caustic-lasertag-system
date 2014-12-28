/*
 * system-time.hpp
 *
 *  Created on: 28 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_SYSTEM_TIME_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_SYSTEM_TIME_HPP_

#include <stdint.h>

class ISystemClock
{
public:
	virtual void wait_us(uint32_t time) = 0;
	virtual uint32_t getTime() = 0;
};

extern ISystemClock *systemClock;



#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_SYSTEM_TIME_HPP_ */

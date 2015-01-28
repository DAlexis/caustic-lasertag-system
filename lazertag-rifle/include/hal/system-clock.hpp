/*
 * system-time.hpp
 *
 *  Created on: 28 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_SYSTEM_TIME_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_SYSTEM_TIME_HPP_

#include <stdint.h>

using Time = uint32_t;

class ISystemClock
{
public:
	virtual void wait_us(uint32_t time) = 0;
	virtual Time getTime() = 0;
};

extern ISystemClock *systemClock;



#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_SYSTEM_TIME_HPP_ */

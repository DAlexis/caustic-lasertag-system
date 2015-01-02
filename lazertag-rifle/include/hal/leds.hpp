/*
 * led.hpp
 *
 *  Created on: 03 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HAL_LED_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HAL_LED_HPP_

#include <stdint.h>

class ILedManager
{
public:
	virtual ~ILedManager() {}
	virtual void ledOn() = 0;
	virtual void ledOff() = 0;
};

class ILedsPool
{
public:
	virtual ~ILedsPool() {}
	virtual ILedManager* getLed(uint8_t ledNumber) = 0;
};

extern ILedsPool* ledsPool;

#endif /* LAZERTAG_RIFLE_INCLUDE_HAL_LED_HPP_ */

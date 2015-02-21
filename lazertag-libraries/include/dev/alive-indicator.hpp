/*
 * alive-indicator.hpp
 *
 *  Created on: 03 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_ALIVE_INDICATOR_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_ALIVE_INDICATOR_HPP_

#include "hal/leds.hpp"

class AliveIndicator
{
public:
	void init(ILedManager* led);
	void blink();
private:
	ILedManager* m_led = nullptr;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_ALIVE_INDICATOR_HPP_ */

/*
 * alive-led.cpp
 *
 *  Created on: 03 янв. 2015 г.
 *      Author: alexey
 */

#include "dev/alive-indicator.hpp"

void AliveIndicator::init(ILedManager* led)
{
	m_led = led;
}

void AliveIndicator::blink()
{
	if (m_led->state())
		m_led->ledOff();
	else
		m_led->ledOn();
}

/*
 * random.cpp
 *
 *  Created on: 01 февр. 2015 г.
 *      Author: alexey
 */

#include "dev/random.hpp"

#include "hal/system-clock.hpp"

#include <stdio.h>

uint32_t Random::m_lastSeed = 0;

uint16_t Random::random16()
{
	return 0xFFFF & random32();
}

uint32_t Random::random32()
{
	m_lastSeed = 0xFFFFFFFF & ( (m_lastSeed+1) * (systemClock->getTime() & 0xFFFF));
	return m_lastSeed;
}

uint32_t Random::random(uint32_t maxValue)
{
	return (uint32_t) (( (double) random32()) / UINT32_MAX * maxValue);
}

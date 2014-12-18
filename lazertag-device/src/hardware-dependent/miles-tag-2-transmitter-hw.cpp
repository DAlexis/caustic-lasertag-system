/*
 * miles-tag-2-transmitter-hw.cpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#include "hardware-dependent/miles-tag-2-hw.hpp"
#include "hardware-dependent/fire-led.hpp"
#include "miles-tag-2-timings.h"

////////////////////////
// MilesTag2Transmitter

void MilesTag2Transmitter::init()
{
	fireLED.setCallback(fireCallbackStaticWrapper, this);
}


MilesTag2Transmitter::MilesTag2Transmitter()
{
}

void MilesTag2Transmitter::beginTransmission()
{
	cursorToStart();
	m_sendingHeader = true;
    fireLED.startImpulsePack(true, HADER_PERIOD);
}

void MilesTag2Transmitter::fireCallback(bool wasOnState)
{
	if (m_sendingHeader)
	{
		m_sendingHeader = false;
		fireLED.startImpulsePack(false, BIT_WAIT_PERIOD);
		return;
	}

	if (wasOnState)
	{
		if (m_currentLength == m_length)
			return;
		fireLED.startImpulsePack(false, BIT_WAIT_PERIOD);
	} else {
		if (nextBit())
			fireLED.startImpulsePack(true, BIT_ONE_PERIOD);
		else
			fireLED.startImpulsePack(true, BIT_ZERO_PERIOD);
	}
}

void MilesTag2Transmitter::fireCallbackStaticWrapper(void* object, bool wasOnState)
{

	reinterpret_cast<MilesTag2Transmitter*>(object)->fireCallback(wasOnState);
}
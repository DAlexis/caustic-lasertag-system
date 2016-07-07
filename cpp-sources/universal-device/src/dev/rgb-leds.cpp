/*
 * rgb-leds.cpp
 *
 *  Created on: 30 марта 2015 г.
 *      Author: alexey
 */

#include "dev/rgb-leds.hpp"

#include <stdio.h>

RGBLeds::RGBLeds(const TeamMT2Id& teamId) :
	m_teamId(teamId)
{
	m_blinkingTask.setTask(std::bind(&RGBLeds::blinkingTask, this));
	m_blinkingTask.setStackSize(128);
	m_blinkingTask.setName("LEDBlink");
	m_blinkingTask.run();
}

void RGBLeds::init(IIOPin* red, IIOPin* green, IIOPin* blue)
{
	m_red = red;
	m_green = green;
	m_blue = blue;
	m_red->switchToOutput();
	m_green->switchToOutput();
	m_blue->switchToOutput();
}

void RGBLeds::changeState(uint8_t state)
{
	if (state & red)
		m_red->set();
	else
		m_red->reset();

	if (state & green)
		m_green->set();
	else
		m_green->reset();

	if (state & blue)
		m_blue->set();
	else
		m_blue->reset();
}

void RGBLeds::blinkingTask()
{
	if (blinksCount != 0)
	{
		changeState(m_color);
		osDelay(m_currentPattern.onStateDuration / 1000);
		changeState(silence);
		osDelay(m_currentPattern.offStateDuration / 1000);
		blinksCount--;
	} else {
		Kernel::yeld();
	}
}

void RGBLeds::blink(const BlinkPattern& pattern, uint8_t color)
{
	if (color == selfColor)
		m_color = getTeamColor();
	else
		m_color = color;

	m_currentPattern = pattern;
	changeState(silence);
	blinksCount = m_currentPattern.repetitionsCount;
}

uint8_t RGBLeds::getTeamColor()
{
	return getTeamColor(m_teamId);
}

uint8_t RGBLeds::getTeamColor(TeamMT2Id teamId)
{
	switch (teamId)
	{
	case 0: return RGBLeds::red;
	case 1: return RGBLeds::blue;
	case 2: return RGBLeds::red | RGBLeds::green;
	case 3: return RGBLeds::green;
	default: return RGBLeds::red | RGBLeds::green | RGBLeds::blue;
	}
}

/*
 * rgb-leds.cpp
 *
 *  Created on: 30 марта 2015 г.
 *      Author: alexey
 */

#include "dev/rgb-leds.hpp"

#include <stdio.h>

RGBLeds::RGBLeds()
{
	m_blinkingTask.setTask(std::bind(&RGBLeds::blinkingTask, this));
	m_blinkingTask.setStackSize(128);
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

void RGBLeds::setColor(uint8_t color)
{
	m_color = color;
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
	changeState(m_color);
	osDelay(m_currentPattern.onStateDuration / 1000);
	changeState(silence);
	osDelay(m_currentPattern.offStateDuration / 1000);
}

void RGBLeds::blink(const BlinkPattern& pattern)
{
	m_blinkingTask.stopUnsafe();
	m_currentPattern = pattern;
	changeState(silence);
	m_blinkingTask.run(0, 0, 0, m_currentPattern.repetitionsCount);
}

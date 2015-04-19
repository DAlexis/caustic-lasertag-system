/*
 * rgb-leds.cpp
 *
 *  Created on: 30 марта 2015 г.
 *      Author: alexey
 */

#include "dev/rgb-leds.hpp"
#include "core/scheduler.hpp"

#include <stdio.h>

RGBLeds::RGBLeds()
{}

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

void RGBLeds::blink(const BlinkPattern& pattern)
{
	stop();
	m_currentPattern = pattern;
	m_state = S_OFF;
	m_currentRepetitionsCount = 0;
	nextStep();
}

void RGBLeds::stop()
{
	changeState(silence);
	if (m_lastTask != 0)
		Scheduler::instance().stopTask(m_lastTask);
}

void RGBLeds::nextStep()
{
	//printf("Next step\n");
	if (m_state == S_OFF)
	{
		if (m_currentRepetitionsCount >= m_currentPattern.repetitionsCount && !m_currentPattern.infinite)
		{
			m_lastTask = 0;
			return;
		}
		m_state = S_ON;
		changeState(m_color);
		m_lastTask = Scheduler::instance().addTask(std::bind(&RGBLeds::nextStep, this), true, 0, 0, m_currentPattern.onStateDuration);
	} else {
		m_currentRepetitionsCount++;
		m_state = S_OFF;
		changeState(silence);
		m_lastTask = Scheduler::instance().addTask(std::bind(&RGBLeds::nextStep, this), true, 0, 0, m_currentPattern.offStateDuration);
	}
}

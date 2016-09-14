/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#include "dev/rgb-leds.hpp"

#include <stdio.h>

RGBLeds::RGBLeds(const TeamGameId& teamId) :
	m_teamId(teamId)
{
	m_blinkingTask.setTask(std::bind(&RGBLeds::blinkingTask, this));
	m_blinkingTask.setStackSize(128);
	m_blinkingTask.setName("LEDBlink");
}

void RGBLeds::init(IIOPin* red, IIOPin* green, IIOPin* blue)
{
	m_red = red;
	m_green = green;
	m_blue = blue;
	m_red->switchToOutput();
	m_green->switchToOutput();
	m_blue->switchToOutput();
	m_red->reset();
	m_green->reset();
	m_blue->reset();
	m_blinkingTask.run();
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

uint8_t RGBLeds::getTeamColor(TeamGameId teamId)
{
	switch (teamId)
	{
	case 0: return RGBLeds::red;
	case 1: return RGBLeds::blue;
	case 2: return RGBLeds::red | RGBLeds::green;
	case 3: return RGBLeds::green;
	default: return RGBLeds::silence;
	}
}

void RGBLeds::stop()
{
	blinksCount = 0;
}

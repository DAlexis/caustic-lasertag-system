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

#ifndef INCLUDE_DEV_RGB_LEDS_HPP_
#define INCLUDE_DEV_RGB_LEDS_HPP_

#include "hal/io-pins.hpp"
#include "core/os-wrappers.hpp"
#include "ir/MT2-base-types.hpp"
#include <stdint.h>

class RGBLeds
{
public:
	constexpr static uint8_t silence = 0;
	constexpr static uint8_t red     = 1;
	constexpr static uint8_t green   = 2;
	constexpr static uint8_t blue    = 4;
	constexpr static uint8_t selfColor    = 255;

	struct BlinkPattern
	{
		BlinkPattern(
				uint32_t _onStateDuration = 200000,
				uint32_t _offStateDuration = 200000,
				unsigned int _repetitionsCount = 2
				) :
			onStateDuration(_onStateDuration),
			offStateDuration(_offStateDuration),
			repetitionsCount(_repetitionsCount)
		{}

		uint32_t onStateDuration;
		uint32_t offStateDuration;
		unsigned int repetitionsCount;
		bool infinite = false;
	};

	RGBLeds(const TeamMT2Id& teamId);
	void init(IIOPin* red, IIOPin* green, IIOPin* blue);
	void blink(const BlinkPattern& pattern, uint8_t color = selfColor);
	void stop();

	uint8_t getTeamColor();
	uint8_t getTeamColor(TeamMT2Id teamId);

private:

	void changeState(uint8_t state);
	void blinkingTask();

	IIOPin* m_red = nullptr;
	IIOPin* m_green = nullptr;
	IIOPin* m_blue = nullptr;

	uint8_t m_color = red;

	const TeamMT2Id& m_teamId;
	BlinkPattern m_currentPattern;

	unsigned int blinksCount = 0;
	TaskCycled m_blinkingTask;
};



#endif /* INCLUDE_DEV_RGB_LEDS_HPP_ */

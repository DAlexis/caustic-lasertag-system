/*
 * rgb-leds.hpp
 *
 *  Created on: 30 марта 2015 г.
 *      Author: alexey
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

/*
 * rgb-leds.hpp
 *
 *  Created on: 30 марта 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_DEV_RGB_LEDS_HPP_
#define INCLUDE_DEV_RGB_LEDS_HPP_

#include "hal/io-pins.hpp"
#include "core/scheduler.hpp"
#include <stdint.h>

class RGBLeds
{
public:
	constexpr static uint8_t silence = 0;
	constexpr static uint8_t red     = 1;
	constexpr static uint8_t green   = 2;
	constexpr static uint8_t blue    = 4;

	RGBLeds();
	void init(IIOPin* red, IIOPin* green, IIOPin* blue);

	void blink(uint32_t onStateDuration, uint32_t offStateDuration, unsigned int repetitionsCount, bool infinite = false);
	void stop();
	void setColor(uint8_t color);

private:
	enum State
	{
		S_OFF = 0,
		S_ON
	};

	void changeState(uint8_t state);
	void nextStep();

	State m_state = S_OFF;

	IIOPin* m_red = nullptr;
	IIOPin* m_green = nullptr;
	IIOPin* m_blue = nullptr;

	uint8_t m_color = red;

	uint32_t m_onStateDuration = 1000000;
	uint32_t m_offStateDuration = 1000000;
	bool m_infinite = false;
	unsigned int m_repetitionsCount = 10;
	unsigned int m_currentRepetitionsCount = 0;

	TaskId m_lastTask = 0;
};



#endif /* INCLUDE_DEV_RGB_LEDS_HPP_ */

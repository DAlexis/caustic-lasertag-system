/*
 * leds.hpp
 *
 *  Created on: 03 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_LEDS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_LEDS_HPP_

#include "hal/leds.hpp"
#include "stm32f10x.h"

#define MAX_LEDS_COUNT		16

class LedManager : public ILedManager
{
public:
	LedManager(GPIO_TypeDef* gpio, uint16_t pinMask) :
		m_gpio(gpio),
		m_pinMask(pinMask)
	{}
	~LedManager() {}

	void ledOn();
	void ledOff();

private:
	GPIO_TypeDef* m_gpio;
	uint16_t m_pinMask;
};

class LedsPool : public ILedsPool
{
public:
	LedsPool();
	ILedManager* getLed(uint8_t ledNumber);
private:
	void createLed(uint8_t ledNumber);
	ILedManager* m_ledManagers[MAX_LEDS_COUNT];
};

#endif /* LAZERTAG_RIFLE_INCLUDE_HW_LEDS_HPP_ */

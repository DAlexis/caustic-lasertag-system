/*
 * input-interrogators.hpp
 *
 *  Created on: 04 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_INPUT_INTERROGATORS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_INPUT_INTERROGATORS_HPP_

#include "hal/inputs-interrogators.hpp"
#include "stm32f10x.h"
#include "map"

class InputInterrogator : public IInputInterrogator
{
public:
	InputInterrogator(GPIO_TypeDef* gpio, uint16_t pinMask) :
		m_gpio(gpio), m_pinMask(pinMask)
	{}

	bool interrogate();

private:
	GPIO_TypeDef* m_gpio;
	uint16_t m_pinMask;
};

class InputInterrogatorsPool : public IInputInterrogatorsPool
{
public:
	IInputInterrogator* getInputInterrogator(uint8_t portNumber, uint8_t pinNumber);

private:
	void createInputInterrogator(uint8_t portNumber, uint8_t pinNumber);
	std::map<std::pair<uint8_t, uint8_t>, InputInterrogator*> m_interrogators;
};




#endif /* LAZERTAG_RIFLE_INCLUDE_HW_INPUT_INTERROGATORS_HPP_ */

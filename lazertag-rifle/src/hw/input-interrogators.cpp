/*
 * input-interrogators.cpp
 *
 *  Created on: 04 янв. 2015 г.
 *      Author: alexey
 */

#include "hw/input-interrogators.hpp"
#include "hw/periph-mapping-helper.h"

InputInterrogatorsPool inputInterrogatorsPoolInstance;
IInputInterrogatorsPool *inputInterrogators = &inputInterrogatorsPoolInstance;

bool InputInterrogator::interrogate()
{
	return GPIO_ReadInputDataBit(m_gpio, m_pinMask) ? true : false;
}

IInputInterrogator* InputInterrogatorsPool::getInputInterrogator(uint8_t portNumber, uint8_t pinNumber)
{
	auto it = m_interrogators.find(std::pair<uint8_t, uint8_t>(portNumber, pinNumber));
	if (it == m_interrogators.end())
	{
		createInputInterrogator(portNumber, pinNumber);
		it = m_interrogators.find(std::pair<uint8_t, uint8_t>(portNumber, pinNumber));
	}
	return it->second;
}

void InputInterrogatorsPool::createInputInterrogator(uint8_t portNumber, uint8_t pinNumber)
{
	RCC_APB2PeriphClockCmd(RCC_MASKx(portNumber), ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_MASK(pinNumber);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOx(portNumber), &GPIO_InitStructure);
	m_interrogators[std::pair<uint8_t, uint8_t>(portNumber, pinNumber)] = new InputInterrogator(GPIOx(portNumber), PIN_MASK(pinNumber));
}

/*
 * input-interrogators.cpp
 *
 *  Created on: 04 янв. 2015 г.
 *      Author: alexey
 */

#include "hw/io-pins.hpp"
#include "hw/periph-mapping-helper.h"

IOPinsPool IOPinsPoolInstance;
IIOPinsPool *IOPins = &IOPinsPoolInstance;

IOPin::IOPin(GPIO_TypeDef* gpio, uint16_t pinMask) :
	m_gpio(gpio), m_pinMask(pinMask)
{
	switchToInput();
}

bool IOPin::state()
{
	return GPIO_ReadInputDataBit(m_gpio, m_pinMask) ? true : false;
}

void IOPin::set()
{
	GPIO_SetBits(m_gpio, m_pinMask);
}

void IOPin::reset()
{
	GPIO_ResetBits(m_gpio, m_pinMask);
}

void IOPin::switchToOutput()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = m_pinMask;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(m_gpio, &GPIO_InitStructure);
}

void IOPin::switchToInput()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = m_pinMask;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(m_gpio, &GPIO_InitStructure);
}

IIOPin* IOPinsPool::getIOPin(uint8_t portNumber, uint8_t pinNumber)
{
	auto it = m_interrogators.find(std::pair<uint8_t, uint8_t>(portNumber, pinNumber));
	if (it == m_interrogators.end())
	{
		createIOPin(portNumber, pinNumber);
		it = m_interrogators.find(std::pair<uint8_t, uint8_t>(portNumber, pinNumber));
	}
	return it->second;
}

void IOPinsPool::createIOPin(uint8_t portNumber, uint8_t pinNumber)
{
	RCC_APB2PeriphClockCmd(RCC_MASKx(portNumber), ENABLE);
	m_interrogators[std::pair<uint8_t, uint8_t>(portNumber, pinNumber)] = new IOPin(GPIOx(portNumber), PIN_MASK(pinNumber));
}

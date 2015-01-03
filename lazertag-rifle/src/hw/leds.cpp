/*
 * leds.cpp
 *
 *  Created on: 03 янв. 2015 г.
 *      Author: alexey
 */

#include "hw/leds.hpp"

/*
 * LEDs mapping:
 * 1..3  -  PC13..15
 * 4..10 -  PC0..6
 */

LedsPool ledsPoolInstance;
ILedsPool* ledsPool = &ledsPoolInstance;

void LedManager::ledOn()
{
	GPIO_SetBits(m_gpio, m_pinMask);
}

void LedManager::ledOff()
{
	GPIO_ResetBits(m_gpio, m_pinMask);
}

bool LedManager::state()
{
	return GPIO_ReadInputDataBit(m_gpio, m_pinMask) ? true : false;
}

LedsPool::LedsPool()
{
	for (int i=0; i<MAX_LEDS_COUNT; i++)
		m_ledManagers[i] = nullptr;
}

ILedManager* LedsPool::getLed(uint8_t ledNumber)
{
	if (!m_ledManagers[ledNumber])
		createLed(ledNumber);

	return m_ledManagers[ledNumber];
}

void LedsPool::createLed(uint8_t ledNumber)
{
	uint16_t pinMask = 0;
	if (ledNumber <= 3)
		pinMask = 1 << (ledNumber+12);
	else
		pinMask = 1 << (ledNumber-4);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = pinMask;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	m_ledManagers[ledNumber] = new LedManager(GPIOC, pinMask);
}

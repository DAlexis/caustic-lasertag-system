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


#include "hw/impulse-emitter-hw.hpp"
#include "core/string-utils.hpp"
#include <stdio.h>
#include "stm32f10x.h"
#include <math.h>

PwmIsrImpulseEmittersPool fireEmittersPoolInstance;
IImpulseEmittersPool* fireEmittersPool = &fireEmittersPoolInstance;

PwmIsrImpulseEmitter* defaultLEDFireEmitter = nullptr;

PwmIsrImpulseEmitter::PwmIsrImpulseEmitter() :
        m_isOn(false)
{
	defaultLEDFireEmitter = this;
}

void PwmIsrImpulseEmitter::init(const Pinout& pinout)
{
	//////////////////////
	// GPIO initialization - PWM output pins
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//////////////////////
	// TIM3 initialization - 38/40/56kHz pulse through PWM
	// Getting system clock frequency
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//Time initialization: setting frequency to default value (56KHz)
	setCarrierFrequency(m_carrierFrequency);

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	modulationOff();

	//////////////////////
	// TIM7 initialization - "video impulse" former
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	m_videoPrescaler = RCC_Clocks.HCLK_Frequency / 1000000;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//NVIC_SetPriority(TIM7_IRQn, 0);
	//NVIC_EnableIRQ(TIM7_IRQn);

	//TIM_ITConfig(TIM7, TIM_DIER_UIE, ENABLE);

	// Creating power level channels
	const char parameterNames[][25] = {
			"IRPowerLevel1Channel",
			"IRPowerLevel2Channel",
			"IRPowerLevel3Channel",
			"IRPowerLevel4Channel",
	};
	for (int i=0; i<4; i++)
	{
		auto result = pinout.getParameter(parameterNames[i]);
		if (result) {
			m_powerChannels[m_powerLevelsCount++] = StringParser<uint8_t>::parse(result.details.c_str());
		}
	}
	setPower(100);
}

void PwmIsrImpulseEmitter::startImpulsePack(bool isLedOn, unsigned int delayMs)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = m_videoPrescaler-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInitStructure.TIM_Period = delayMs-1;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStructure);

	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

	TIM_ITConfig(TIM7, TIM_DIER_UIE, ENABLE);

	m_isOn = isLedOn;
	if (isLedOn)
		modulationOn();
	else
		modulationOff();

	TIM_Cmd(TIM7, ENABLE);
}

void PwmIsrImpulseEmitter::setCarrierFrequency(uint32_t frequency)
{
	m_carrierFrequency = frequency;

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	m_radioPrescaler = RCC_Clocks.HCLK_Frequency / m_carrierFrequency / m_radioTimerPeriod;

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = m_radioPrescaler-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInitStructure.TIM_Period = m_radioTimerPeriod-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
}

void PwmIsrImpulseEmitter::setPower(UintParameter powerPercent)
{
	if (m_powerLevelsCount == 0)
		return;
	unsigned int powerLevel = ceil(powerPercent / 100.0 * m_powerLevelsCount) - 1;

	if (powerLevel > m_powerLevelsCount)
		powerLevel = m_powerLevelsCount-1;

	//printf("SHOT channel: %d\n", m_powerChannels[powerLevel]);
	setChannel(m_powerChannels[powerLevel]);
}

void PwmIsrImpulseEmitter::setChannel(unsigned int channel)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	// Disabling all output channels
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = m_radioTimerPeriod / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	// Enabling one output channel
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	m_PWMChannel = channel;
	switch(m_PWMChannel)
	{
	default:
	case 1: TIM_OC1Init(TIM3, &TIM_OCInitStructure); break;
	case 2: TIM_OC2Init(TIM3, &TIM_OCInitStructure); break;
	case 3: TIM_OC3Init(TIM3, &TIM_OCInitStructure); break;
	case 4: TIM_OC4Init(TIM3, &TIM_OCInitStructure); break;
	}

	// What does this function do?
	//TIM_ARRPreloadConfig(TIM3, ENABLE);
}

void PwmIsrImpulseEmitter::modulationOn()
{
	if (m_powerLevelsCount == 0)
		return;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = m_radioTimerPeriod / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	switch(m_PWMChannel)
	{
	case 1: TIM_OC1Init(TIM3, &TIM_OCInitStructure); break;
	case 2: TIM_OC2Init(TIM3, &TIM_OCInitStructure); break;
	case 3: TIM_OC3Init(TIM3, &TIM_OCInitStructure); break;
	case 4: TIM_OC4Init(TIM3, &TIM_OCInitStructure); break;
	}
	TIM_Cmd(TIM3, ENABLE);

	//TIM_CtrlPWMOutputs(TIM3, ENABLE);
}

void PwmIsrImpulseEmitter::modulationOff()
{
	if (m_powerLevelsCount == 0)
		return;
	TIM_Cmd(TIM3, DISABLE);
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = m_radioTimerPeriod / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	switch(m_PWMChannel)
	{
	case 1: TIM_OC1Init(TIM3, &TIM_OCInitStructure); break;
	case 2: TIM_OC2Init(TIM3, &TIM_OCInitStructure); break;
	case 3: TIM_OC3Init(TIM3, &TIM_OCInitStructure); break;
	case 4: TIM_OC4Init(TIM3, &TIM_OCInitStructure); break;
	}
}

void PwmIsrImpulseEmitter::IRQHandler()
{
	modulationOff();
	TIM_Cmd(TIM7, DISABLE);
	TIM_ITConfig(TIM7, TIM_DIER_UIE, DISABLE);
	if (m_callback)
		m_callback(m_isOn);
	else
		printf("Fire LED callback not set!\n");
}


extern "C" void TIM7_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
        if (defaultLEDFireEmitter)
        	defaultLEDFireEmitter->IRQHandler();
    }
}


//////////////////////
// FireEmittersPool
IImpulseEmitter* PwmIsrImpulseEmittersPool::get()
{
	if (!m_emitter)
		m_emitter = new PwmIsrImpulseEmitter;
	return m_emitter;
}

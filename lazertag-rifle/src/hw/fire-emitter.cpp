/*
 * fire-emitter.cpp
 *
 *  Created on: 26 дек. 2014 г.
 *      Author: alexey
 */

#include "hw/fire-emitter.hpp"
#include "stm32f10x.h"
#include <stdio.h>

FireEmittersPool fireEmittersPoolInstance;
IFireEmittersPool* fireEmittersPool = &fireEmittersPoolInstance;

LEDFireEmitter* defaultLEDFireEmitter = nullptr;

LEDFireEmitter::LEDFireEmitter() :
        m_isOn(false)
{
	defaultLEDFireEmitter = this;
}

void LEDFireEmitter::init()
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

	setPower(100);
	modulationOff();

	//////////////////////
	// TIM7 initialization - "video impulse" former
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	m_videoPrescaler = RCC_Clocks.HCLK_Frequency / 1000000;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	NVIC_EnableIRQ(TIM7_IRQn);
	TIM_ITConfig(TIM7, TIM_DIER_UIE, ENABLE);
}

void LEDFireEmitter::startImpulsePack(bool isLedOn, unsigned int delayMs)
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

void LEDFireEmitter::setCarrierFrequency(uint32_t frequency)
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

void LEDFireEmitter::setPower(uint8_t powerPercent)
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
	if (powerPercent <= 25)
	{
		m_PWMChannel = 1;
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	}
	else if (powerPercent <= 50)
	{
		m_PWMChannel = 2;
		TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	}
	else if (powerPercent <= 75)
	{
		m_PWMChannel = 3;
		TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	}
	else
	{
		m_PWMChannel = 4;
		TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	}

	// What does this function do?
	//TIM_ARRPreloadConfig(TIM3, ENABLE);
}

void LEDFireEmitter::modulationOn()
{

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

void LEDFireEmitter::modulationOff()
{
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

void LEDFireEmitter::IRQHandler()
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
IFireEmitter* FireEmittersPool::getFireEmitter(uint8_t)
{
	if (!m_emitter)
		m_emitter = new LEDFireEmitter;
	return m_emitter;
}

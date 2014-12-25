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
        m_videoPrescaler(1000),
        m_isOn(false),
		m_powerLevel(24)
{
	defaultLEDFireEmitter = this;
}

void LEDFireEmitter::init()
{
	//////////////////////
	// GPIO initialization - PWM output pins
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//////////////////////
	// TIM3 initialization - 38/40/56kHz pulse through PWM
	// Getting system clock frequency
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	/// @todo Improve this place and remove copy-paste in modulation On/Off funcs
	const unsigned int pulseFreq = 56000;
	const unsigned int timerPeriod = 10;

	unsigned int fillingPrescaler = RCC_Clocks.HCLK_Frequency / pulseFreq / timerPeriod;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//Time initialization
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = fillingPrescaler-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInitStructure.TIM_Period = timerPeriod-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = timerPeriod / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// What this function does?
	//TIM_ARRPreloadConfig(TIM3, ENABLE);

	modulationOff();

	//////////////////////
	// TIM7 initialization - "video impulse" former
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

	//TIM15->CNT = 0;

	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

	TIM_ITConfig(TIM7, TIM_DIER_UIE, ENABLE);

	m_isOn = isLedOn;
	if (isLedOn)
		modulationOn();
	else
		modulationOff();

	TIM_Cmd(TIM7, ENABLE);
}

void LEDFireEmitter::setPower(uint8_t powerPercent)
{

}

void LEDFireEmitter::modulationOn()
{
	TIM_Cmd(TIM3, ENABLE);
	const unsigned int timerPeriod = 10;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = timerPeriod / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	//TIM_CtrlPWMOutputs(TIM3, ENABLE);
}

void LEDFireEmitter::modulationOff()
{
	TIM_Cmd(TIM3, DISABLE);
	const unsigned int timerPeriod = 10;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = timerPeriod / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	//TIM_CtrlPWMOutputs(TIM3, DISABLE);
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

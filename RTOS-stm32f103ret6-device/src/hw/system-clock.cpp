/*
 * system-clock.cpp
 *
 *  Created on: 29 дек. 2014 г.
 *      Author: alexey
 */

#include "hw/system-clock.hpp"

#include "stm32f10x.h"

SystemClock clock;
ISystemClock *systemClock = &clock;

constexpr uint32_t msInSec = 1000000;
constexpr uint32_t msTimerPeriod = UINT16_MAX;
constexpr uint32_t slaveTimerPeriod = UINT16_MAX;

SystemClock::SystemClock()
{

}

void SystemClock::init()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	//SysTick_Config(RCC_Clocks.HCLK_Frequency / SYSTICK_FREQUENCY_HZ);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	// Initializing microseconds timer
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;//SystemCoreClock / msInSec - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInitStructure.TIM_Period = msTimerPeriod;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
	TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);

	// Initializing slave timer
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInitStructure.TIM_Period = slaveTimerPeriod;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);

	TIM_SelectSlaveMode(TIM5, TIM_SlaveMode_Gated);
	TIM_SelectInputTrigger(TIM5, TIM_TS_ITR2);


	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
}

Time SystemClock::getTime()
{
	uint16_t slave = TIM5->CNT;
	uint16_t us = TIM4->CNT;

	if (us < 3) {
		us = TIM4->CNT;
		slave = TIM5->CNT;
	}

	Time result = slave * msTimerPeriod + us;

	return result;
}

void SystemClock::wait_us(uint32_t time)
{
	Time lastTime = getTime();
	while (getTime()-lastTime < time) { }

}

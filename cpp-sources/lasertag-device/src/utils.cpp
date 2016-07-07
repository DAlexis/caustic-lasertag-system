/*
 * utils.cpp
 *
 *  Created on: 25 окт. 2014 г.
 *      Author: alexey
 */

#include "utils.hpp"
#include "stm32f10x.h"

#include <stdint.h>

constexpr uint32_t msInSec = 1000000;
constexpr uint32_t msTimerPeriod = UINT16_MAX;

constexpr uint32_t slaveTimerPeriod = UINT16_MAX;

constexpr uint32_t msTimerClockSource = 24000000;


SysTicTimer systemTimer;

SysTicTimer::SysTicTimer() :
    m_testTime(0)
{
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    //SysTick_Config(RCC_Clocks.HCLK_Frequency / SYSTICK_FREQUENCY_HZ);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    // Initializing microseconds timer
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Prescaler = msTimerClockSource / msInSec - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInitStructure.TIM_Period = msTimerPeriod;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

    // Initializing slave timer
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInitStructure.TIM_Period = slaveTimerPeriod;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);
    TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1);


    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void SysTicTimer::delay(__IO uint32_t timeToWait)
{
    volatile uint32_t beginTime = getTime();
    while (beginTime + timeToWait > getTime()) ;

}

void SysTicTimer::interrupt()
{
    time++;
}

uint32_t SysTicTimer::getTime()
{
    return getTime2();
}

uint32_t SysTicTimer::getTime2()
{/*
    uint16_t us=TIM2->CNT;
    uint16_t slave = TIM3->CNT;

    uint16_t us2=TIM2->CNT;
    uint16_t slave2 = TIM3->CNT;

    if (us2 != us || slave != slave2)
    {
        us=TIM2->CNT;
        slave = TIM3->CNT;
    }
*/
    uint16_t slave = TIM3->CNT;
    uint16_t us = TIM2->CNT;

    if (us < 3) {
        us = TIM2->CNT;
        slave = TIM3->CNT;
    }

    uint32_t result = slave * msTimerPeriod + us;

    return result;
}

uint32_t SysTicTimer::getMs()
{
    return TIM2->CNT;
}

uint32_t SysTicTimer::getSlave()
{
    return TIM3->CNT;
}

void SysTicTimer::testTimeSmoothness()
{
    m_testLastTime = m_testTime;
    m_testTime = systemTimer.getTime();
    if (m_testTime < m_testLastTime)
        printf("jump backward - last: %u current: %u, diff: %d ms: %u, slave: %u\n", m_testLastTime, m_testTime, (int)m_testLastTime-m_testTime, getMs(), getSlave());
    else if (m_testTime - m_testLastTime > 65000)
        printf("jump forward - last: %u current: %u, diff: %d ms: %u, slave: %u\n", m_testLastTime, m_testTime, (int)m_testTime -m_testLastTime, getMs(), getSlave());

}

/*
extern "C"
{

    void SysTick_Handler(void)
    {
        systemTimer.interrupt();
    }

}
*/

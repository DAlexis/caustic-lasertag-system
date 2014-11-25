/*
 * utils.cpp
 *
 *  Created on: 25 окт. 2014 г.
 *      Author: alexey
 */

#include "utils.hpp"
#include "stm32f10x.h"

SysTicTimer systemTimer;

SysTicTimer::SysTicTimer() :
    time(0)
{
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / SYSTICK_FREQUENCY_HZ);
}

void SysTicTimer::delay(__IO uint32_t timeToWait)
{
    __IO uint32_t beginTime = time;
    while (beginTime + timeToWait > time) ;

}

void SysTicTimer::interrupt()
{
    time++;
}

uint32_t SysTicTimer::getTime()
{
    return time;
}

extern "C"
{

    void SysTick_Handler(void)
    {
        systemTimer.interrupt();
    }

}

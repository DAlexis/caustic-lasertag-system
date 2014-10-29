/*
 * utils.cpp
 *
 *  Created on: 25 окт. 2014 г.
 *      Author: alexey
 */

#include "utils.hpp"
#include "stm32f10x.h"

SysTicTimer systemTimer;

SysTicTimer::SysTicTimer()
{
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / SYSTICK_FREQUENCY_HZ);
}

void SysTicTimer::delay(__IO uint32_t time)
{
    m_counter = time;

    while (m_counter != 0) ;

}

void SysTicTimer::decrement()
{
    if (m_counter != 0)
        m_counter--;
}

extern "C"
{

    void SysTick_Handler(void)
    {
        systemTimer.decrement();
    }

}

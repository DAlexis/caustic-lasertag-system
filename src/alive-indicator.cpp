/*
 * alive-indicator.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */

#include "alive-indicator.hpp"
#include "utils.hpp"
#include "stm32f10x.h"

AliveIndicator::AliveIndicator()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

void AliveIndicator::blink()
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    for (volatile int i=0; i<100000; i++) { }
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
    for (volatile int i=0; i<100000; i++) { }
}

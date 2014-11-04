/*
 * fire-led.cpp
 *
 *  Created on: 04 нояб. 2014 г.
 *      Author: alexey
 */


#include "fire-led.hpp"
#include "stm32f10x.h"

#include <stdio.h>

FireLEDManager fireLED;

FireLEDManager::FireLEDManager() :
        m_videoPrescaler(1000),
        m_callback(nullptr),
        m_callbackObject(nullptr)
{
}

void FireLEDManager::init()
{
    //////////////////////
    // GPIO initialization - PWM output pins
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //////////////////////
    // TIM17 initialization - 38/40/56kHz pulse

    // Getting system clock frequency
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    const unsigned int pulseFreq = 56000;
    const unsigned int timerPeriod = 10;

    unsigned int fillingPrescaler = RCC_Clocks.HCLK_Frequency / pulseFreq / timerPeriod;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);

    //Time initialization
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Prescaler = fillingPrescaler-1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInitStructure.TIM_Period = timerPeriod-1;
    TIM_TimeBaseInit(TIM17,&TIM_TimeBaseInitStructure);

    TIM_OCInitTypeDef TIM_OCInitStructure;// = {0,};
    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = timerPeriod / 2;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    //TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM17,&TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM17, TIM_OCPreload_Enable);

    modulationOff();

    //////////////////////
    // TIM15 initialization "video impulse" former
    m_videoPrescaler = RCC_Clocks.HCLK_Frequency / 1000000;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);

    TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
    NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
    TIM_ITConfig(TIM15, TIM_DIER_UIE, ENABLE);
}

void FireLEDManager::setCallback(FireLEDMgrCallback callback, void* object)
{
    m_callback = callback;
    m_callbackObject = object;
}

void FireLEDManager::startImpulsePack(BlinkingState state, unsigned int delayMs)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Prescaler = m_videoPrescaler-1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInitStructure.TIM_Period = delayMs-1;
    TIM_TimeBaseInit(TIM15,&TIM_TimeBaseInitStructure);

    if (state == BS_ON)
        modulationOn();
    else
        modulationOff();

    TIM_Cmd(TIM15, ENABLE);
}

void FireLEDManager::modulationOn()
{
    TIM_Cmd(TIM17, ENABLE);
    TIM_CtrlPWMOutputs(TIM17, ENABLE);
}

void FireLEDManager::modulationOff()
{
    TIM_CtrlPWMOutputs(TIM17, DISABLE);
    TIM_Cmd(TIM17, DISABLE);
}

void FireLEDManager::IRQHandler()
{
    TIM_Cmd(TIM15, DISABLE);
    modulationOff();
    if (m_callback)
        m_callback(m_callbackObject);
    else
        printf("Fire LED callback not set!\n");
}

extern "C" void TIM1_BRK_TIM15_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM15, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
        fireLED.IRQHandler();
    }
}

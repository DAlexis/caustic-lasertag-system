/*
 * buttons-maanger.cpp
 *
 *  Created on: 21 нояб. 2014 г.
 *      Author: alexey
 */

#include "buttons-manager.hpp"
#include "utils.hpp"
#include "exti-initialization-options.h"
#include "stm32f10x.h"




ButtonsManager buttons;

ButtonsManager::ButtonsManager()
{
    for (unsigned int i=0; i<BUTTONS_COUNT; i++)
    {
        m_callbacks[i] = nullptr;
        m_buttonLastPressed[i] = 0;
    }
}

void ButtonsManager::configButton(unsigned int button, ButtonAutoRepeat autoRepeat, unsigned int minRepeatPeriod)
{
    m_buttonPeriod[button] = minRepeatPeriod;
    m_autoRepeat[button] = autoRepeat;

    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(inputDescription[button].RCC_APB2Periph_GPIOx, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = inputDescription[button].GPIO_Pin_x;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(inputDescription[button].GPIOx, &GPIO_InitStructure);

    m_buttonLastState[button] = GPIO_ReadInputDataBit(inputDescription[button].GPIOx, inputDescription[button].GPIO_Pin_x);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(inputDescription[button].GPIO_PortSourceGPIOx, inputDescription[button].GPIO_PinSourcex);

    // Configure EXTIx line
    EXTI_InitStructure.EXTI_Line = inputDescription[button].EXTI_Linex;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

    EXTI_Init(&EXTI_InitStructure);

    // Enable and set EXTIx Interrupt to the lowest priority
    NVIC_InitStructure.NVIC_IRQChannel = inputDescription[button].NVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void ButtonsManager::setButtonCallback(unsigned int button, ButtonCallback callback, void* callbackObject)
{
    m_callbackObjects[button] = callbackObject;
    m_callbacks[button] = callback;
}

void ButtonsManager::interruptionHandler(unsigned int button)
{
    unsigned int time = systemTimer.getTime();

    if (time - m_buttonLastPressed[button] >= m_buttonPeriod[button])
    {
        m_buttonLastPressed[button] = time;
        m_callbacks[button] (m_callbackObjects[button], true);
    }
}

void ButtonsManager::doAction(unsigned int button)
{
}

void ButtonsManager::interrogateButton(unsigned int button)
{
    // @todo Add critical section here
    if (m_autoRepeat[button] == BUTTON_AUTO_REPEAT_DISABLE)
        return;

    unsigned int time = systemTimer.getTime();
    uint8_t state = GPIO_ReadInputDataBit(inputDescription[button].GPIOx, inputDescription[button].GPIO_Pin_x);
    if (time - m_buttonLastPressed[button] >= m_buttonPeriod[button] && state == 0)
    {
        m_buttonLastPressed[button] = time;
        m_callbacks[button] (m_callbackObjects[button], false);
    }
}

void ButtonsManager::interrogateAllButtons()
{
    for (unsigned int i=0; i<BUTTONS_COUNT; i++)
    {
        if (m_callbacks[i])
            interrogateButton(i);
    }
}



extern "C" void EXTI0_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
        buttons.interruptionHandler(0);
    }
}


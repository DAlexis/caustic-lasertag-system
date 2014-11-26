/*
 * buttons-maanger.cpp
 *
 *  Created on: 21 нояб. 2014 г.
 *      Author: alexey
 */

#include "buttons-manager.hpp"
#include "utils.hpp"
#include "stm32f10x.h"

struct ButtonDescription
{
    uint32_t RCC_APB2Periph_GPIOx;
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin_x;
    uint8_t GPIO_PortSourceGPIOx;
    uint8_t GPIO_PinSourcex;
    uint32_t EXTI_Linex;
    uint8_t NVIC_IRQChannel;
};

static constexpr ButtonDescription buttonDescr[] = {
    {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_0, GPIO_PortSourceGPIOB, GPIO_PinSource0, EXTI_Line0, EXTI0_IRQn},
    {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_1, GPIO_PortSourceGPIOB, GPIO_PinSource1, EXTI_Line1, EXTI1_IRQn},
    {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_2, GPIO_PortSourceGPIOB, GPIO_PinSource2, EXTI_Line2, EXTI2_IRQn},
    {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_3, GPIO_PortSourceGPIOB, GPIO_PinSource3, EXTI_Line3, EXTI3_IRQn},
};

ButtonsManager buttons;

ButtonsManager::ButtonsManager()
{
    for (unsigned int i=0; i<BUTTONS_COUNT; i++)
    {
        m_callbacks[i] = nullptr;
        m_buttonLastPressed[i] = 0;
    }
}

void ButtonsManager::configButton(unsigned int button, ButtonCallbackCase callbackCase, ButtonAutoRepeat autoRepeat, unsigned int minRepeatPeriod)
{
    m_buttonPeriod[button] = minRepeatPeriod;
    m_callbackCase[button] = callbackCase;
    m_autoRepeat[button] = autoRepeat;

    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    // @todo Add support for auto repeating (only if manually interrogated)

    RCC_APB2PeriphClockCmd(buttonDescr[button].RCC_APB2Periph_GPIOx, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = buttonDescr[button].GPIO_Pin_x;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(buttonDescr[button].GPIOx, &GPIO_InitStructure);

    m_buttonLastState[button] = GPIO_ReadInputDataBit(buttonDescr[button].GPIOx, buttonDescr[button].GPIO_Pin_x);

    if (autoRepeat == BUTTON_AUTO_REPEAT_DISABLE)
    {
        if (autoRepeat == BUTTON_AUTO_REPEAT_DISABLE)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

            GPIO_EXTILineConfig(buttonDescr[button].GPIO_PortSourceGPIOx, buttonDescr[button].GPIO_PinSourcex);

            // Configure EXTI10 line
            EXTI_InitStructure.EXTI_Line = buttonDescr[button].EXTI_Linex;
            EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
            EXTI_InitStructure.EXTI_LineCmd = ENABLE;
            switch (callbackCase)
            {
            case BCC_DISABLED: EXTI_InitStructure.EXTI_LineCmd = DISABLE;
            case BCC_HIGH: EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; break;
            case BCC_LOW: EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; break;
            case BCC_BOTH: EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; break;
            }

            EXTI_Init(&EXTI_InitStructure);

            // Enable and set EXTI10 Interrupt to the lowest priority
            NVIC_InitStructure.NVIC_IRQChannel = buttonDescr[button].NVIC_IRQChannel;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
        }
    }
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
        doAction(button);
    }
}

void ButtonsManager::doAction(unsigned int button)
{
}

void ButtonsManager::interrogateButton(unsigned int button)
{
    if (m_autoRepeat[button] == BUTTON_AUTO_REPEAT_DISABLE)
        return;

    unsigned int time = systemTimer.getTime();
    if (time - m_buttonLastPressed[button] >= m_buttonPeriod[button])
    {
        m_buttonLastPressed[button] = time;
        uint8_t state = GPIO_ReadInputDataBit(buttonDescr[button].GPIOx, buttonDescr[button].GPIO_Pin_x);
        if (state != 0 && m_callbackCase[button] == BCC_HIGH)
            m_callbacks[button] (m_callbackObjects[button], true);

        else if (state == 0 && m_callbackCase[button] == BCC_LOW)
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


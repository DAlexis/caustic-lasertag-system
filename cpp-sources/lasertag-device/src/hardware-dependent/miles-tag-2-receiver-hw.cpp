/*
 * miles-tag-2-receiver-hw.cpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#include "hardware-dependent/miles-tag-2-hw.hpp"
#include "miles-tag-2-timings.h"
#include "exti-initialization-options.h"
#include "utils.hpp"

#include "stm32f10x.h"
#include <stdio.h>

MilesTag2Receiver* pMT2ReceiverForCallback;

MilesTag2Receiver::MilesTag2Receiver()
{
	pMT2ReceiverForCallback = this;
    resetReceiver();
}

void MilesTag2Receiver::init(unsigned int channel)
{
    m_channel = channel;
    m_state = RS_WAITING_HEADER;
    resetReceiver();

    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(inputDescription[channel].RCC_APB2Periph_GPIOx, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = inputDescription[channel].GPIO_Pin_x;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(inputDescription[channel].GPIOx, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(inputDescription[channel].GPIO_PortSourceGPIOx, inputDescription[channel].GPIO_PinSourcex);

    // Configure EXTIx line
    EXTI_InitStructure.EXTI_Line = inputDescription[channel].EXTI_Linex;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

    EXTI_Init(&EXTI_InitStructure);

    // Enable and set EXTIx Interrupt to the lowest priority
    NVIC_InitStructure.NVIC_IRQChannel = inputDescription[channel].NVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void MilesTag2Receiver::interruptionHandler()
{
    uint8_t status = GPIO_ReadInputDataBit(inputDescription[m_channel].GPIOx, inputDescription[m_channel].GPIO_Pin_x);
    unsigned int time = systemTimer.getTime();
    unsigned int dtime = time - m_lastTime;
    if (m_debug) printf("\ni %d %d %d ", (int) status, (int) dtime, (int)m_state);
    if (dtime < 15) {
        if (m_debug) printf ("short");
        m_falseImpulse = true;
        return;
    }

    if (m_falseImpulse) {
        if (m_debug) printf ("short back");
        m_falseImpulse = false;
        return;
    }
    m_lastTime = time;

    switch(m_state) {
    case RS_WAITING_HEADER:
        // We should have 1
        if (status == 0) {
            resetReceiver();
            return;
        }

        if (m_debug) printf ("hb ");
        // Header beginned
        m_state = RS_HEADER_BEGINNED;
        break;

    case RS_HEADER_BEGINNED: {
            // We should have 0
            if (status != 0)
            {
                resetReceiver();
                return;
            }

            if (m_debug) printf ("he ");
            if (isCorrect(dtime, HEADER_PERIOD_MIN, HEADER_PERIOD_MAX)) {
                if (m_debug) printf("ac \n");
                m_state = RS_SPACE;

            } else {
                resetReceiver();
                return;
            }
        } break;

    case RS_SPACE: {
            // We should have 1
            if (status == 0)
            {
                resetReceiver();
                return;
            }

            if (m_debug) printf ("sp ");
            if (isCorrect(dtime, BIT_WAIT_PERIOD_MIN, BIT_WAIT_PERIOD_MAX)) {
                if (m_debug) printf("ac \n");
                m_state = RS_BIT;
            } else {
                resetReceiver();
                return;
            }
        } break;

    case RS_BIT: {
            if (status != 0)
            {
                resetReceiver();
                return;
            }
            if (m_debug) printf ("b ");
            if (isCorrect(dtime, BIT_ONE_PERIOD_MIN, BIT_ONE_PERIOD_MAX)) {
                if (m_debug) printf("ac 1 \n");
                saveBit(true);
                m_state = RS_SPACE;
                if (getCurrentLength() == 14)
                    m_dataReady = true;
            } else if (isCorrect(dtime, BIT_ZERO_PERIOD_MIN, BIT_ZERO_PERIOD_MAX)) {
                if (m_debug) printf("ac 0 \n");
                saveBit(false);
                m_state = RS_SPACE;
                if (getCurrentLength() == 14)
                    m_dataReady = true;
            } else {
                resetReceiver();
                return;
            }
        } break;
    }
}

void MilesTag2Receiver::resetReceiver()
{
    m_pCurrentByte = m_data;
    m_currentBit = 7;
    m_state = RS_WAITING_HEADER;
    m_falseImpulse = false;
    m_dataReady = false;
}

extern "C" void EXTI4_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
    	pMT2ReceiverForCallback->interruptionHandler();
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}
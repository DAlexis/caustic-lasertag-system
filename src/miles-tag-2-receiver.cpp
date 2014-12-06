/*
 * miles-tag-2.cpp
 *
 *  Created on: 05 нояб. 2014 г.
 *      Author: alexey
 */

#include "miles-tag-2.hpp"
#include "exti-initialization-options.h"
#include "miles-tag-2-timings.h"
#include "utils.hpp"
#include "stm32f10x.h"
#include <stdio.h>

MilesTag2Receiver milesTag2Receiver;

/////////////////////
// Receiver
MilesTag2Receiver::MilesTag2Receiver() :
    m_debug(false),
    m_shotCallback(nullptr),
    m_shotObject(nullptr),
    m_pCurrentByte(m_data),
    m_currentBit(7)
{
    resetReceiverBuffer();
}

void MilesTag2Receiver::init(unsigned int channel)
{
    m_channel = channel;
    m_state = RS_WAITING_HEADER;
    resetReceiverBuffer();

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

void MilesTag2Receiver::setShortMessageCallback(MilesTag2ShotCallback callback, void* object)
{
    m_shotCallback = callback;
    m_shotObject = object;
}

inline bool MilesTag2Receiver::isCorrect(unsigned int value, unsigned int min, unsigned int max)
{
    return (value > min && value < max);
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
            resetReceiverBuffer();
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
                resetReceiverBuffer();
                return;
            }

            if (m_debug) printf ("he ");
            if (isCorrect(dtime, HEADER_PERIOD_MIN, HEADER_PERIOD_MAX)) {
                if (m_debug) printf("ac \n");
                m_state = RS_SPACE;

            } else {
                resetReceiverBuffer();
                return;
            }
        } break;

    case RS_SPACE: {
            // We should have 1
            if (status == 0)
            {
                resetReceiverBuffer();
                return;
            }

            if (m_debug) printf ("sp ");
            if (isCorrect(dtime, BIT_WAIT_PERIOD_MIN, BIT_WAIT_PERIOD_MAX)) {
                if (m_debug) printf("ac \n");
                m_state = RS_BIT;
            } else {
                resetReceiverBuffer();
                return;
            }
        } break;

    case RS_BIT: {
            if (status != 0)
            {
                resetReceiverBuffer();
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
                resetReceiverBuffer();
                return;
            }
        } break;
    }
}

void MilesTag2Receiver::interrogate()
{
    if (getCurrentLength() == 14 && getBit(0) == false) {
        parseAndCallShot();
        resetReceiverBuffer();
        // We have shot mesage
    }

    /// @todo determine message type
    /*
    if (!m_dataReady)
        return;

    m_dataReady = false;
    m_shortMessageCallback(m_shortMessageObject, m_data);

    resetReceiverBuffer();
    */
}

void MilesTag2Receiver::parseAndCallShot()
{
    unsigned int playerId   = m_data[0] & ~(1 << 7);
    unsigned int teamId     = m_data[1] >> 6;
    unsigned int damageCode = (m_data[1] & 0b00111100) >> 2;
    m_shotCallback(m_shotObject, teamId, playerId, damageCode);
}

bool MilesTag2Receiver::getBit(unsigned int n)
{
    return m_data[n / 8] & (1 << (7 - n%8));
}

void MilesTag2Receiver::saveBit(bool value)
{
    if (m_pCurrentByte - m_data == MILESTAG2_MAX_MESSAGE_LENGTH)
        return;
    if (value)
        *m_pCurrentByte |= (1 << m_currentBit);
    else
        *m_pCurrentByte &= ~(1 << m_currentBit);

    if (m_currentBit == 0) {
        m_currentBit = 7;
        m_pCurrentByte++;
    } else
        m_currentBit--;
}

void MilesTag2Receiver::resetReceiverBuffer()
{
    m_pCurrentByte = m_data;
    m_currentBit = 7;
    m_state = RS_WAITING_HEADER;
    m_falseImpulse = false;
    m_dataReady = false;
}

int MilesTag2Receiver::getCurrentLength()
{
    return (m_pCurrentByte - m_data)*8 + 7-m_currentBit;
}

void MilesTag2Receiver::enableDebug(bool debug)
{
    m_debug = debug;
}

uint8_t MilesTag2Receiver::decodeDamage(uint8_t damage)
{
    switch(damage)
    {
    case 0: return 1;
    case 1: return 2;
    case 2: return 4;
    case 3: return 5;
    case 4: return 7;
    case 5: return 10;
    case 6: return 15;
    case 7: return 17;
    case 8: return 20;
    case 9: return 25;
    case 10: return 30;
    case 11: return 35;
    case 12: return 40;
    case 13: return 50;
    case 14: return 75;
    case 15: return 100;
    default: return 0;
    }
}

extern "C" void EXTI4_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        milesTag2Receiver.interruptionHandler();
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

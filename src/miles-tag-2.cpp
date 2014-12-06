/*
 * miles-tag-2.cpp
 *
 *  Created on: 05 нояб. 2014 г.
 *      Author: alexey
 */

#include "miles-tag-2.hpp"
#include "fire-led.hpp"
#include "exti-initialization-options.h"
#include "utils.hpp"
#include "stm32f10x.h"
#include <stdio.h>

#define SHOT_LENGTH             14

#define HADER_PERIOD            2400
#define BIT_ONE_PERIOD          1200
#define BIT_ZERO_PERIOD         600
#define BIT_WAIT_PERIOD         600
/*
#define HEADER_PERIOD_MIN       200
#define HEADER_PERIOD_MAX       260

#define BIT_ONE_PERIOD_MIN      100
#define BIT_ONE_PERIOD_MAX      140

#define BIT_ZERO_PERIOD_MIN     50
#define BIT_ZERO_PERIOD_MAX     70

#define BIT_WAIT_PERIOD_MIN     50
#define BIT_WAIT_PERIOD_MAX     70

#define WAIT_AFTER_END          200
*/


#define HEADER_PERIOD_MIN       190
#define HEADER_PERIOD_MAX       260

#define BIT_ONE_PERIOD_MIN      75
#define BIT_ONE_PERIOD_MAX      110

#define BIT_ZERO_PERIOD_MIN     35
#define BIT_ZERO_PERIOD_MAX     45

#define BIT_WAIT_PERIOD_MIN     40
#define BIT_WAIT_PERIOD_MAX     55

#define WAIT_AFTER_END          200

MilesTag2Transmitter milesTag2;
MilesTag2Receiver milesTag2Receiver;

/////////////////////
// Transmitter

MilesTag2Transmitter::MilesTag2Transmitter() :
    m_pCurrentByte(m_data),
    m_currentBit(7),
    m_playerId(15),
    m_teamId(0),
    m_length(0),
    m_currentLength(0)
{
}

void MilesTag2Transmitter::setPlayerId(uint8_t playerId)
{
    m_playerId = playerId;
}

void MilesTag2Transmitter::setTeamId(uint8_t teamId)
{
    m_teamId = teamId;
}

void MilesTag2Transmitter::init()
{
    fireLED.setCallback(fireCallback, this);
}

void MilesTag2Transmitter::fireCallback(void* object, bool wasOnState)
{
    MilesTag2Transmitter *pThis = reinterpret_cast<MilesTag2Transmitter*>(object);
    if (pThis->m_sendingHeader)
    {
        pThis->m_sendingHeader = false;
        fireLED.startImpulsePack(false, BIT_WAIT_PERIOD);
        return;
    }

    if (wasOnState)
    {
        if (pThis->m_currentLength == pThis->m_length)
            return;
        fireLED.startImpulsePack(false, BIT_WAIT_PERIOD);
    } else {
        if (pThis->nextBit())
            fireLED.startImpulsePack(true, BIT_ONE_PERIOD);
        else
            fireLED.startImpulsePack(true, BIT_ZERO_PERIOD);
    }
}


void MilesTag2Transmitter::shot(uint8_t damage)
{
    // Forming data package
    m_length = SHOT_LENGTH;
    m_currentLength = 0;
    m_data[0] = m_playerId & 0b01111111;
    m_data[1] = (m_teamId << 6) | ( (damage & 0b00001111) << 2);
    m_currentBit = 7;
    m_pCurrentByte = m_data;
    m_sendingHeader = true;

    fireLED.startImpulsePack(true, HADER_PERIOD);
}

bool MilesTag2Transmitter::nextBit()
{
    bool result = (*m_pCurrentByte & (1 << m_currentBit)) != 0;
    m_currentLength++;
    if (m_currentBit == 0) {
        m_currentBit = 7;
        m_pCurrentByte++;
    } else
        m_currentBit--;
    return result;
}

/////////////////////
// Receiver
MilesTag2Receiver::MilesTag2Receiver() :
    m_shortMessageCallback(nullptr),
    m_shortMessageObject(nullptr),
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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void MilesTag2Receiver::setShortMessageCallback(MilesTag2ShortMessageCallback callback, void* object)
{
    m_shortMessageCallback = callback;
    m_shortMessageObject = object;
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
    printf("\ni %d %d %d ", (int) status, (int) dtime, (int)m_state);
   // m_lastTime = time;
    if (dtime < 15) {
        printf ("short");
        m_falseImpulse = true;
        return;
    }

    if (m_falseImpulse) {
        printf ("short back");
        m_falseImpulse = false;
        return;
    }
    m_lastTime = time;
    //return;

    switch(m_state) {
    case RS_WAITING_HEADER:
        // We should have 1
        if (status == 0) {
            resetReceiverBuffer();
            return;
        }

        printf ("hb ");
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

            printf ("he ");
            if (isCorrect(dtime, HEADER_PERIOD_MIN, HEADER_PERIOD_MAX)) {
                printf("ac \n");
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

            printf ("sp ");
            if (isCorrect(dtime, BIT_WAIT_PERIOD_MIN, BIT_WAIT_PERIOD_MAX)) {
                printf("ac \n");
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

            printf ("b ");
            if (isCorrect(dtime, BIT_ONE_PERIOD_MIN, BIT_ONE_PERIOD_MAX)) {
                printf("a1 \n");
                saveBit(true);
                m_state = RS_SPACE;
            } else if (isCorrect(dtime, BIT_ZERO_PERIOD_MIN, BIT_ZERO_PERIOD_MAX)) {
                printf("a0 \n");
                saveBit(false);
                m_state = RS_SPACE;
            } else {
                resetReceiverBuffer();
                return;
            }
        } break;
    }
}

void MilesTag2Receiver::interrogate()
{
    /** Checking if enough time passed after last pulse and running callback
     */
    if (systemTimer.getTime() - m_lastTime > WAIT_AFTER_END) {
        if (m_state == RS_SPACE)
            m_shortMessageCallback(m_shortMessageObject, m_data);
        /// @todo determine message type

        // It is another state, so bad sygnal
        resetReceiverBuffer();
    }

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
}


extern "C" void EXTI4_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        milesTag2Receiver.interruptionHandler();
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

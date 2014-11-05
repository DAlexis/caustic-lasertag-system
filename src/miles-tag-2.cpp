/*
 * miles-tag-2.cpp
 *
 *  Created on: 05 нояб. 2014 г.
 *      Author: alexey
 */

#include "miles-tag-2.hpp"
#include "fire-led.hpp"

#define SHOT_LENGTH             14

#define HADER_PERIOD            2400
#define BIT_ONE_PERIOD          1200
#define BIT_ZERO_PERIOD         600
#define BIT_WAIT_PERIOD         600

MilesTag2Transmitter milesTag2;

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

/*
 * miles-tag-2-transmitter.cpp
 *
 *  Created on: 06 дек. 2014 г.
 *      Author: alexey
 */


#include "hal/miles-tag-2-base.hpp"
#include "utils.hpp"
#include <stdio.h>


#define SHOT_LENGTH             14
#define MESSAGE_LENGTH          (3*8)

#define MESSAGE_END_BYTE        0xE8

IMilesTag2Transmitter* milesTag2;

/////////////////////
// Transmitter
MilesTag2TransmitterBase::MilesTag2TransmitterBase() :
	m_pCurrentByte(m_data),
	m_currentBit(7),
	m_currentLength(0),
    m_length(0),
	m_playerId(15),
	m_teamId(0)

{
}

void MilesTag2TransmitterBase::setPlayerId(uint8_t playerId)
{
    m_playerId = playerId;
}

void MilesTag2TransmitterBase::setTeamId(uint8_t teamId)
{
    m_teamId = teamId;
}

void MilesTag2TransmitterBase::shot(uint8_t damage)
{
    // Forming data package
    m_length = SHOT_LENGTH;
    m_data[0] = m_playerId & 0b01111111;
    m_data[1] = (m_teamId << 6) | ( (damage & 0b00001111) << 2);
    beginTransmission();
}



bool MilesTag2TransmitterBase::nextBit()
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

uint8_t MilesTag2TransmitterBase::encodeDamage(uint8_t damage)
{
    if (damage == 1)
        return 0;

    if (damage == 2)
        return 1;

    if (damage >= 3 && damage < 5)
        return 2;

    if (damage == 5)
        return 3;

    if (damage >= 6 && damage < 9)
        return 4;

    if (damage >= 9 && damage < 14)
        return 5;

    if (damage >= 14 && damage < 16)
        return 6;

    if (damage >= 16 && damage < 19)
        return 7;

    if (damage >= 19 && damage < 23)
        return 8;

    if (damage >= 23 && damage < 28)
        return 9;

    if (damage >= 28 && damage < 33)
        return 10;

    if (damage >= 33 && damage < 38)
        return 11;

    if (damage >= 38 && damage < 45)
        return 12;

    if (damage >= 45 && damage < 58)
        return 13;

    if (damage >= 58 && damage < 88)
        return 14;

    if (damage >= 88)
        return 15;

    // To avoid warning
    return 0;
}

void MilesTag2TransmitterBase::addHealth(uint8_t value)
{
    m_length = MESSAGE_LENGTH;
    m_data[0] = 0x80;
    m_data[1] = value;
    m_data[3] = MESSAGE_END_BYTE;
    beginTransmission();
}

void MilesTag2TransmitterBase::addRounds(uint8_t value)
{
    m_length = MESSAGE_LENGTH;
    m_data[0] = 0x81;
    m_data[1] = value;
    m_data[3] = MESSAGE_END_BYTE;
    beginTransmission();
}

void MilesTag2TransmitterBase::sendCommand(uint8_t commandCode)
{
    m_length = MESSAGE_LENGTH;
    m_data[0] = 0x83;
    m_data[1] = commandCode;
    m_data[3] = MESSAGE_END_BYTE;
    beginTransmission();
}

void MilesTag2TransmitterBase::adminKill()
{
    sendCommand(0x00);
}

void MilesTag2TransmitterBase::pauseOrUnpause()
{
    sendCommand(0x01);
}

void MilesTag2TransmitterBase::startGame()
{
    sendCommand(0x02);
}

void MilesTag2TransmitterBase::restoreDefaults()
{
    sendCommand(0x03);
}

void MilesTag2TransmitterBase::respawn()
{
    sendCommand(0x04);
}

void MilesTag2TransmitterBase::newGameImmediate()
{
    sendCommand(0x05);
}

void MilesTag2TransmitterBase::fullAmmo()
{
    sendCommand(0x06);
}

void MilesTag2TransmitterBase::endGame()
{
    sendCommand(0x07);
}

void MilesTag2TransmitterBase::resetClock()
{
    sendCommand(0x08);
}

void MilesTag2TransmitterBase::initializePlayer()
{
    sendCommand(0x0A);
}

void MilesTag2TransmitterBase::explodePlayer()
{
    sendCommand(0x0B);
}

void MilesTag2TransmitterBase::newGameReady()
{
    sendCommand(0x0C);
}

void MilesTag2TransmitterBase::fullHealth()
{
    sendCommand(0x0D);
}

void MilesTag2TransmitterBase::fullArmor()
{
    sendCommand(0x0F);
}

void MilesTag2TransmitterBase::clearScores()
{
    sendCommand(0x14);
}

void MilesTag2TransmitterBase::testSensors()
{
    sendCommand(0x15);
}

void MilesTag2TransmitterBase::stunPlayer()
{
    sendCommand(0x16);
}

void MilesTag2TransmitterBase::disarmPlayer()
{
    sendCommand(0x17);
}


void MilesTag2TransmitterBase::ammoPickup(uint8_t ammoBoxId)
{
    m_length = MESSAGE_LENGTH;
    m_data[0] = 0x8A;
    m_data[1] = ammoBoxId;
    m_data[3] = MESSAGE_END_BYTE;
    beginTransmission();
}

void MilesTag2TransmitterBase::healthPickup(uint8_t healthBoxId)
{
    m_length = MESSAGE_LENGTH;
    m_data[0] = 0x8B;
    m_data[1] = healthBoxId;
    m_data[3] = MESSAGE_END_BYTE;
    beginTransmission();
}

void MilesTag2TransmitterBase::flagPickup(uint8_t flagBoxId)
{
    m_length = MESSAGE_LENGTH;
    m_data[0] = 0x8C;
    m_data[1] = flagBoxId;
    m_data[3] = MESSAGE_END_BYTE;
    beginTransmission();
}


void MilesTag2TransmitterBase::cursorToStart()
{
	m_currentLength = 0;
	m_currentBit = 7;
	m_pCurrentByte = m_data;
}


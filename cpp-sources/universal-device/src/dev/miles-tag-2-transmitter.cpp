/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/


#include <dev/miles-tag-details.hpp>
#include "dev/miles-tag-2.hpp"
#include "core/logging.hpp"
#include <stdio.h>



/////////////////////
// Transmitter
MilesTag2Transmitter::MilesTag2Transmitter(const UintParameter& power) :
	m_pCurrentByte(m_data),
	m_currentBit(7),
	m_currentLength(0),
 	m_length(0),
	m_power(power)
{
}

void MilesTag2Transmitter::setPlayerIdReference(uint8_t& playerId)
{
    m_playerId = &playerId;
}

void MilesTag2Transmitter::setTeamIdReference(uint8_t& teamId)
{
    m_teamId = &teamId;
}

void MilesTag2Transmitter::shot(uint8_t damage)
{
    // Forming data package
    m_length = MT2Extended::shotLength;
    m_data[0] = *m_playerId & 0b01111111;
    m_data[1] = (*m_teamId << 6) | ( (encodeDamage(damage) & 0b00001111) << 2);
    beginTransmission();
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

uint8_t MilesTag2Transmitter::encodeDamage(uint8_t damage)
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

void MilesTag2Transmitter::addHealth(int16_t value)
{
    m_length = MT2Extended::messageLength;
    m_data[0] = MT2Extended::Byte1::addHealth;
    m_data[1] = MT2Extended::encodeAddHealth(value);
    m_data[2] = MT2Extended::Byte3::messageEnd;
    beginTransmission();
}

void MilesTag2Transmitter::setTeam(uint8_t teamId)
{
	m_length = MT2Extended::messageLength;
	m_data[0] = MT2Extended::Byte1::setTeam;
	m_data[1] = teamId & 0x03;
	m_data[2] = MT2Extended::Byte3::messageEnd;
	beginTransmission();
}


void MilesTag2Transmitter::addRounds(uint8_t value)
{
    m_length = MT2Extended::messageLength;
    m_data[0] = MT2Extended::Byte1::addRounds;
    m_data[1] = value;
    m_data[2] = MT2Extended::Byte3::messageEnd;
    beginTransmission();
}

void MilesTag2Transmitter::sendCommand(uint8_t commandCode)
{
    m_length = MT2Extended::commandLength;
    m_data[0] = MT2Extended::Byte1::command;
    m_data[1] = commandCode;
    m_data[2] = MT2Extended::Byte3::commandEnd;
    beginTransmission();
}

void MilesTag2Transmitter::adminKill()
{
    sendCommand(MT2Extended::Commands::adminKill);
}

void MilesTag2Transmitter::pauseOrUnpause()
{
    sendCommand(MT2Extended::Commands::pauseOrUnpause);
}

void MilesTag2Transmitter::startGame()
{
    sendCommand(MT2Extended::Commands::startGame);
}

void MilesTag2Transmitter::restoreDefaults()
{
    sendCommand(MT2Extended::Commands::restoreDefaults);
}

void MilesTag2Transmitter::respawn()
{
    sendCommand(MT2Extended::Commands::respawn);
}

void MilesTag2Transmitter::newGameImmediate()
{
    sendCommand(MT2Extended::Commands::newGameImmediate);
}

void MilesTag2Transmitter::fullAmmo()
{
    sendCommand(MT2Extended::Commands::fullAmmo);
}

void MilesTag2Transmitter::endGame()
{
    sendCommand(MT2Extended::Commands::endGame);
}

void MilesTag2Transmitter::resetClock()
{
    sendCommand(MT2Extended::Commands::resetClock);
}

void MilesTag2Transmitter::initializePlayer()
{
    sendCommand(MT2Extended::Commands::initializePlayer);
}

void MilesTag2Transmitter::explodePlayer()
{
    sendCommand(MT2Extended::Commands::explodePlayer);
}

void MilesTag2Transmitter::newGameReady()
{
    sendCommand(MT2Extended::Commands::newGameReady);
}

void MilesTag2Transmitter::fullHealth()
{
    sendCommand(MT2Extended::Commands::fullHealth);
}

void MilesTag2Transmitter::fullArmor()
{
    sendCommand(MT2Extended::Commands::fullArmor);
}

void MilesTag2Transmitter::clearScores()
{
    sendCommand(MT2Extended::Commands::clearScores);
}

void MilesTag2Transmitter::testSensors()
{
    sendCommand(MT2Extended::Commands::testSensors);
}

void MilesTag2Transmitter::stunPlayer()
{
    sendCommand(MT2Extended::Commands::stunPlayer);
}

void MilesTag2Transmitter::disarmPlayer()
{
    sendCommand(MT2Extended::Commands::disarmPlayer);
}


void MilesTag2Transmitter::ammoPickup(uint8_t ammoBoxId)
{
    m_length = MT2Extended::messageLength;
    m_data[0] = MT2Extended::Byte1::ammoPickup;
    m_data[1] = ammoBoxId;
    m_data[2] = MT2Extended::Byte3::messageEnd;
    beginTransmission();
}

void MilesTag2Transmitter::healthPickup(uint8_t healthBoxId)
{
    m_length = MT2Extended::messageLength;
    m_data[0] = MT2Extended::Byte1::healthPickup;
    m_data[1] = healthBoxId;
    m_data[2] = MT2Extended::Byte3::messageEnd;
    beginTransmission();
}

void MilesTag2Transmitter::flagPickup(uint8_t flagBoxId)
{
    m_length = MT2Extended::messageLength;
    m_data[0] = MT2Extended::Byte1::flagPickup;
    m_data[1] = flagBoxId;
    m_data[2] = MT2Extended::Byte3::messageEnd;
    beginTransmission();
}


void MilesTag2Transmitter::cursorToStart()
{
	m_currentLength = 0;
	m_currentBit = 7;
	m_pCurrentByte = m_data;
}

///////////////////
void MilesTag2Transmitter::init(const Pinout& pinout, unsigned int fireEmitterNumber)
{
	m_fireEmitter = fireEmittersPool->getFireEmitter(fireEmitterNumber);
	m_fireEmitter->setCallback(std::bind(&MilesTag2Transmitter::fireCallback, this, std::placeholders::_1));
	m_fireEmitter->init(pinout);
}

void MilesTag2Transmitter::beginTransmission()
{
	m_fireEmitter->setPower(m_power);
	cursorToStart();
	m_sendingHeader = true;
	m_fireEmitter->startImpulsePack(true, HADER_PERIOD);
}

void MilesTag2Transmitter::fireCallback(bool wasOnState)
{
	if (m_sendingHeader)
	{
		m_sendingHeader = false;
		m_fireEmitter->startImpulsePack(false, BIT_WAIT_PERIOD);
		return;
	}

	if (wasOnState)
	{
		if (m_currentLength == m_length)
			return;
		m_fireEmitter->startImpulsePack(false, BIT_WAIT_PERIOD);
	} else {
		if (nextBit())
			m_fireEmitter->startImpulsePack(true, BIT_ONE_PERIOD);
		else
			m_fireEmitter->startImpulsePack(true, BIT_ZERO_PERIOD);
	}
}



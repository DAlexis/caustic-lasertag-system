/*
 * miles-tag-2-receiver.cpp
 *
 *  Created on: 23 янв. 2015 г.
 *      Author: alexey
 */

#include "dev/miles-tag-details.h"
#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "dev/miles-tag-2.hpp"
#include "hal/system-clock.hpp"
#include <stdio.h>

void MilesTag2Receiver::setShortMessageCallback(MilesTag2ShotCallback callback)
{
	m_shotCallback = callback;
}

void MilesTag2Receiver::init(IExternalInterruptManager* exti)
{
	m_exti = exti;
	m_exti->setCallback(std::bind(&MilesTag2Receiver::interruptHandler, this, std::placeholders::_1));
	resetReceiver();
}

void MilesTag2Receiver::turnOn()
{
	m_exti->turnOn();
}

void MilesTag2Receiver::turnOff()
{
	m_exti->turnOff();
}

void MilesTag2Receiver::interrogate()
{
	if (m_nextInterrogationCallback)
	{
		m_nextInterrogationCallback();
		m_nextInterrogationCallback = nullptr;
		return;
	}

	parseVariableSizeMessage();
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

bool MilesTag2Receiver::getBit(unsigned int n)
{
	return m_data[n / 8] & (1 << (7 - n%8));
}

bool MilesTag2Receiver::isCorrect(unsigned int value, unsigned int min, unsigned int max)
{
	return (value > min && value < max);
}

int MilesTag2Receiver::getCurrentLength()
{
	return (m_pCurrentByte - m_data)*8 + 7-m_currentBit;
}


bool MilesTag2Receiver::parseConstantSizeMessage()
{
	if (getCurrentLength() == 14 && getBit(0) == false)
	{
		// We have the shot
		unsigned int playerId   = m_data[0] & ~(1 << 7);
		unsigned int teamId     = m_data[1] >> 6;
		unsigned int damageCode = (m_data[1] & 0b00111100) >> 2;
		if (m_shotCallback)
		{
			m_nextInterrogationCallback = std::bind(m_shotCallback, teamId, playerId, decodeDamage(damageCode));
		} else {
			m_nextInterrogationCallback = nullptr;
			printf("Shot callback not set!\n");
		}
		return true;
	}
	else if (getCurrentLength() == 16 && m_data[0] == MT2Std::Byte1::command)
	{
		printf("Command with code %x detected\n", m_data[1]);
		RCSPStream stream;
		switch(m_data[1])
		{
		case MT2Std::Commands::adminKill:
			break;
		case MT2Std::Commands::pauseOrUnpause:
			break;
		case MT2Std::Commands::startGame:
			break;
		case MT2Std::Commands::restoreDefaults:
			break;
		case MT2Std::Commands::respawn:
			stream.addCall(ConfigCodes::Player::Functions::playerRespawn);
			break;
		case MT2Std::Commands::newGameImmediate:
			break;
		case MT2Std::Commands::fullAmmo:
			break;
		case MT2Std::Commands::endGame:
			break;
		case MT2Std::Commands::resetClock:
			break;
		case MT2Std::Commands::initializePlayer:
			break;
		case MT2Std::Commands::explodePlayer:
			break;
		case MT2Std::Commands::newGameReady:
			break;
		case MT2Std::Commands::fullHealth:
			break;
		case MT2Std::Commands::fullArmor:
			break;
		case MT2Std::Commands::clearScores:
			break;
		case MT2Std::Commands::testSensors:
			break;
		case MT2Std::Commands::stunPlayer:
			break;
		case MT2Std::Commands::disarmPlayer:
			break;
		default:
			return false;
		}
		stream.dispatch();
	}
	else
		return false;
}

void MilesTag2Receiver::resetReceiver()
{
	m_pCurrentByte = m_data;
	m_currentBit = 7;
	m_state = RS_WAITING_HEADER;
	m_falseImpulse = false;
	m_dataReady = false;
	m_nextInterrogationCallback = nullptr;
}

void MilesTag2Receiver::interruptHandler(bool state)
{
	unsigned int time = systemClock->getTime();
	unsigned int dtime = time - m_lastTime;
	// Inverted input:
	state = !state;
	if (m_debug) {
		printf("dt=%u ",  dtime );
		if (state)
			printf("1\n");
		else
			printf("0\n");
	}

	if (dtime < 15) {
		if (m_debug) printf ("short\n");
		m_falseImpulse = true;
		return;
	}

	if (m_falseImpulse) {
		if (m_debug) printf ("short back\n");
		m_falseImpulse = false;
		return;
	}
	m_lastTime = time;

	switch(m_state) {
	case RS_WAITING_HEADER:
		// We should have 1
		if (state == 0) {
			resetReceiver();
			return;
		}

		if (m_debug) printf ("hb \n");
		// Header beginned
		m_state = RS_HEADER_BEGINNED;
		break;

	case RS_HEADER_BEGINNED: {
			// We should have 0
			if (state != 0)
			{
				resetReceiver();
				return;
			}

			if (m_debug) printf ("he \n");
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
			if (state == 0)
			{
				resetReceiver();
				return;
			}

			if (m_debug) printf ("sp \n");
			if (isCorrect(dtime, BIT_WAIT_PERIOD_MIN, BIT_WAIT_PERIOD_MAX)) {
				if (m_debug) printf("ac \n");
				m_state = RS_BIT;
			} else {
				resetReceiver();
				return;
			}
		} break;

	case RS_BIT: {
			if (state != 0)
			{
				resetReceiver();
				return;
			}
			if (m_debug) printf ("b \n");
			if (isCorrect(dtime, BIT_ONE_PERIOD_MIN, BIT_ONE_PERIOD_MAX))
			{
				// We have bit "1"
				if (m_debug) printf("ac 1 \n");
				saveBit(true);
				m_state = RS_SPACE;
			} else if (isCorrect(dtime, BIT_ZERO_PERIOD_MIN, BIT_ZERO_PERIOD_MAX)){
				// We have bit "0"
				if (m_debug) printf("ac 0 \n");
				saveBit(false);
				m_state = RS_SPACE;
			} else {
				// Translation broken
				resetReceiver();
				return;
			}
			// Check if we have consistent message with fixed size. This is
			// to prevent loosing information when interrogation period is long
			if (parseConstantSizeMessage())
			{
				// We have consistent message
				resetReceiver();
				return;
			}
		} break;
	}
}



bool MilesTag2Receiver::parseVariableSizeMessage()
{
	unsigned int time = systemClock->getTime();
	if (time - m_lastTime < TIME_BEFORE_PARSING)
		return false;

	if (getCurrentLength() >= 8 + RCSPAggregator::minimalStreamSize
		&& getCurrentLength() % 8 == 0
		&& m_data[0] == RCSP_MESSAGE)
	{
		printf("Infrared: general purpose RCSP message detected\n");
		// We have message for RCSP system
		unsigned int messageSize = getCurrentLength() / 8 - 1;
		uint8_t *message = &m_data[1];
		if (RCSPAggregator::instance().isStreamConsistent(message, messageSize))
			RCSPAggregator::instance().dispatchStream(message, messageSize);
	}
	resetReceiver();
	return false;
}

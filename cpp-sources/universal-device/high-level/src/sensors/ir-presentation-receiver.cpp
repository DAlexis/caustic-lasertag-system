/*
*    Copyright (C) 2017 by Aleksey Bulatov
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

#include "sensors/ir-presentation-receiver.hpp"
#include "rcsp/operation-codes.hpp"
#include "core/logging.hpp"

IRReceiversManager::IRReceiversManager(KillZonesManager& mgr) :
	m_killZonesManager(mgr)
{
}

void IRReceiversManager::connectRCSPAggregator(RCSPAggregator& aggregator)
{
	m_rcspAggregator = &aggregator;
}

void IRReceiversManager::setParser(IIRProtocolParser* parser)
{
	m_parser = parser;
}

void IRReceiversManager::addPhysicalReceiver(IIRReceiverPhysical* receiver)
{
	m_receivers.push_back(receiver);
}

void IRReceiversManager::interrogate()
{
	for (auto it : m_receivers)
	{
		it->interrogate();
		if (it->isDataReady())
		{
			uint8_t* data;
			uint16_t bitsCount;
			it->getData(data, bitsCount);
			debug << "IR receiver with id = " << it->getId()
					<< " has data: " << hexStr(data, bitsCount / 8 + 1);
			m_parser->parse(m_parseRusult, data, bitsCount, it->getId());
			processReceivedResults(it->getId());
			m_lastMessageTime = systemClock->getTime();
		}
	}
	checkTimeout();
}

void IRReceiversManager::processReceivedResults(UintParameter receiverId)
{
	switch(m_state)
	{
	case State::empty:
		switch(m_parseRusult.type)
		{
		case IRProtocolParseResult::Type::shot:
			applyCoefficient(m_parseRusult.shot, receiverId);
			m_currentShotMessage = m_parseRusult.shot;
			m_state = State::hasShot;
			break;
		case IRProtocolParseResult::Type::command:
			m_currentCommand = m_parseRusult.commandCallback;
			m_state = State::hasCommand;
			break;
		case IRProtocolParseResult::Type::invalid:
			break;
		}
		break;
	case State::hasCommand:
		// We should ignore anything here...
		break;
	case State::hasShot:
		switch(m_parseRusult.type)
		{
		case IRProtocolParseResult::Type::shot:
			applyCoefficient(m_parseRusult.shot, receiverId);
			if (m_parseRusult.shot.damage > m_currentShotMessage.damage)
				m_currentShotMessage = m_parseRusult.shot;
			break;
		case IRProtocolParseResult::Type::command:
			// Ignore command...
			break;
		case IRProtocolParseResult::Type::invalid:
			break;
		}
		break;
	}
}

void IRReceiversManager::applyCoefficient(ShotMessage& shot, UintParameter receiverId)
{
	shot.damage *= m_killZonesManager.getDamageCoefficient(receiverId);
}

void IRReceiversManager::checkTimeout()
{
	if (m_state == State::empty)
		return;
	Time t = systemClock->getTime();
	if (t - m_lastMessageTime > timeout)
	{
		if (m_state == State::hasCommand)
		{
			if (m_currentCommand)
				m_currentCommand();
		} else if (m_state == State::hasShot)
		{
			m_rcspAggregator->doOperation(ConfigCodes::HeadSensor::Functions::catchShot, m_currentShotMessage);
		}
		m_state = State::empty;
	}
}

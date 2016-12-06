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


#include "ir/ir-presentation-mt2.hpp"
#include "ir/MT2-details.hpp"
#include "core/logging.hpp"
#include "rcsp/operation-codes.hpp"
#include "hal/system-clock.hpp"
#include "utils/memory.hpp"

FloatParameter IRPresentationReceiverMT2::defaultDamageCoefficient = 1.0;

IRPresentationTransmitterMT2::IRPresentationTransmitterMT2(RCSPAggregator& rcspAggregator) :
	m_rcspAggregator(rcspAggregator)
{

}

void IRPresentationTransmitterMT2::init()
{
}

void IRPresentationTransmitterMT2::sendMessage(const RCSPStream& stream)
{
	RCSPAggregator::Operation op;
	bool success = true;
	m_rcspAggregator.extractNextOperation(stream.getStream(), op, stream.getSize(), success);
	switch(op.code)
	{
		case ConfigCodes::HeadSensor::Functions::catchShot: {
			const ShotMessage* shot = reinterpret_cast<const ShotMessage*>(op.argument);
			m_data[0] = shot->playerId & 0b01111111;
			m_data[1] = (shot->teamId << 6) | ( (encodeDamage(shot->damage) & 0b00001111) << 2);
			m_physicalTransmitter->send(m_data, MT2Extended::shotLength);
		} break;
		case ConfigCodes::HeadSensor::Functions::playerKill: {
			sendCommand(MT2Extended::Commands::adminKill);
		} break;
		case ConfigCodes::HeadSensor::Functions::playerReset: {
			sendCommand(MT2Extended::Commands::startGame);
		} break;
		case ConfigCodes::AnyDevice::Functions::resetToDefaults: {
			sendCommand(MT2Extended::Commands::restoreDefaults);
		} break;
		case ConfigCodes::HeadSensor::Functions::playerRespawn: {
			sendCommand(MT2Extended::Commands::respawn);
		} break;
		default:
			sendGenericRCSPMessageDefault(stream);
	}

}

void IRPresentationTransmitterMT2::sendGenericRCSPMessageDefault(const RCSPStream& stream)
{
	m_data[0] = MT2Extended::Byte1::RCSPMessage;
	if (stream.getSize() > maxMessageSize - 1)
	{
		error << "Too long message for MT2 transmission";
		return;
	}
	memcpy(&(m_data[1]), stream.getStream(), stream.getSize());
	m_physicalTransmitter->send(m_data, stream.getSize()*8);
}


uint8_t IRPresentationTransmitterMT2::encodeDamage(uint8_t damage)
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

void IRPresentationTransmitterMT2::sendCommand(uint8_t commandCode)
{
    m_data[0] = MT2Extended::Byte1::command;
    m_data[1] = commandCode;
    m_data[2] = MT2Extended::Byte3::commandEnd;
    m_physicalTransmitter->send(m_data, MT2Extended::commandLength);
}

// /////////////////////////
// IRPresentationReceiverMT2

IRPresentationReceiverMT2::IRPresentationReceiverMT2(RCSPAggregator& rcspAggregator) :
	m_rcspAggregator(rcspAggregator)
{

}

void IRPresentationReceiverMT2::init()
{

}

void IRPresentationReceiverMT2::assignReceiversGroup(IPresentationReceiversGroup& group)
{
	m_group = static_cast<PresentationReceiversGroupMT2*>(&group);
}

void IRPresentationReceiverMT2::interrogate()
{
	if (m_vibro != nullptr && systemClock->getTime() - m_vibroEnabledTime > vibroPeriod)
	{
		m_vibro->reset();
	}
	m_physicalReceiver->interrogate();
}

void IRPresentationReceiverMT2::setDamageCoefficient(const FloatParameter* coefficient)
{
	m_damageCoefficient = coefficient;
}

void IRPresentationReceiverMT2::receiverCallback(const uint8_t* data, uint16_t size)
{
	debug << "IR receiver has data: ";
	printHex(data, size);
	enableVibro();
	// If shot
	if ((data[0] & ~MT2Extended::Byte1::shotMask) == 0)
	{
		// For compatibility with protocol extensions no limitations to package size are provided
		parseShot(data, size);
		return;
	}
	// Parsing a "command"
	if (size == MT2Extended::commandLength
		&& data[0] == MT2Extended::Byte1::command
		&& data[2] == MT2Extended::Byte3::commandEnd)
	{
		parseMT2Command(data, size);
		return;
	}

	// Team change
	if (size == MT2Extended::messageLength
		&& data[0] == MT2Extended::Byte1::setTeam)
	{
		parseSetTeam(data, size);
		return;
	}

	// Health count changing
	if (size == MT2Extended::messageLength
		&& data[0] == MT2Extended::Byte1::addHealth)
	{
		parseAddHealth(data, size);
		return;
	}

	if (size > 8 + RCSPAggregator::minimalStreamSize
		&& size % 8 == 0
		&& data[0] == MT2Extended::Byte1::RCSPMessage)

	{
		parseRCSP(data, size);
		return;
	}

}

void IRPresentationReceiverMT2::parseShot(const uint8_t* data, uint16_t size)
{
	UNUSED_ARG(size);
	if (size < MT2Extended::shotLength)
	{
		warning << "Shot package with len < " << size << " ignored";
		return;
	}
	ShotMessage msg;
	msg.playerId  = data[0] & ~(1 << 7);
	msg.teamId    = data[1] >> 6;
	unsigned int damageCode = (data[1] & 0b00111100) >> 2;
	msg.damage = decodeDamage(damageCode) * *m_damageCoefficient;
	if (!m_group->m_waitingOtherKillzonesShot || msg.damage > m_group->m_maxDamage)
	{
		m_group->m_shotMessage = msg;
		m_group->m_waitingOtherKillzonesShot = true;
		updateOperationCallback(
			[this]() {
				m_rcspAggregator.doOperation(ConfigCodes::HeadSensor::Functions::catchShot, m_group->m_shotMessage);
			}
		);
	}
}

void IRPresentationReceiverMT2::parseSetTeam(const uint8_t* data, uint16_t size)
{
	UNUSED_ARG(size);
	if (data[1] & ~(0x03)) {
		warning << "Team id byte contains non-zero upper bits";
	}
	TeamGameId team = data[1] & 0x03;
	updateOperationCallback([team, this]() mutable {
		m_rcspAggregator.doOperation(
				ConfigCodes::HeadSensor::Functions::setTeam,
				team);
	});
}

void IRPresentationReceiverMT2::parseAddHealth(const uint8_t* data, uint16_t size)
{
	UNUSED_ARG(size);
	int16_t healthDelta = MT2Extended::decodeAddHealth(data[1]);
	updateOperationCallback([healthDelta, this]() mutable {
		m_rcspAggregator.doOperation(
				ConfigCodes::HeadSensor::Functions::addMaxHealth,
				healthDelta);
	});
}

void IRPresentationReceiverMT2::parseMT2Command(const uint8_t* data, uint16_t size)
{
	UNUSED_ARG(size);
	switch(data[1])
	{
	case MT2Extended::Commands::adminKill:
		updateOperationCallback([this]() { m_rcspAggregator.doOperation(ConfigCodes::HeadSensor::Functions::playerKill); });
		break;
	case MT2Extended::Commands::pauseOrUnpause:
		break;
	case MT2Extended::Commands::startGame:
		updateOperationCallback([this]() { m_rcspAggregator.doOperation(ConfigCodes::HeadSensor::Functions::playerReset); });
		break;
	case MT2Extended::Commands::restoreDefaults:
		updateOperationCallback([this]() { m_rcspAggregator.doOperation(ConfigCodes::AnyDevice::Functions::resetToDefaults); });
		break;
	case MT2Extended::Commands::fullHealth:
	case MT2Extended::Commands::initializePlayer:
	case MT2Extended::Commands::newGameReady:
	case MT2Extended::Commands::newGameImmediate:
	case MT2Extended::Commands::respawn:
		updateOperationCallback([this]() { m_rcspAggregator.doOperation(ConfigCodes::HeadSensor::Functions::playerRespawn); });
		break;
	case MT2Extended::Commands::fullAmmo:
		break;
	case MT2Extended::Commands::endGame:
		break;
	case MT2Extended::Commands::resetClock:
		break;
	case MT2Extended::Commands::explodePlayer:
		break;
	case MT2Extended::Commands::fullArmor:
		break;
	case MT2Extended::Commands::clearScores:
		break;
	case MT2Extended::Commands::testSensors:
		break;
	case MT2Extended::Commands::stunPlayer:
		break;
	case MT2Extended::Commands::disarmPlayer:
		break;
	default:
		return;
	}
}

void IRPresentationReceiverMT2::parseRCSP(const uint8_t* data, uint16_t size)
{
	const uint8_t* stream = &data[1];
	uint16_t streamSize = size-1;
	if (!m_rcspAggregator.isStreamConsistent(stream, streamSize))
	{
		warning << "Inconsistent RCSP stream on IR channel";
		return;
	}
	debug << "General RCSP message by IR receiver";
	if (size > PresentationReceiversGroupMT2::argumentBufferSize)
	{
		error << "RCSP IR stream is too large to be copied to temporary buffer";
		return;
	}
	memcpy(m_group->m_bufferForArgument, stream, streamSize);
	updateOperationCallback([this, streamSize]() mutable {
		m_rcspAggregator.dispatchStream(m_group->m_bufferForArgument, streamSize);
	});
}

void IRPresentationReceiverMT2::enableVibro()
{
	if (m_vibro)
	{
		m_vibro->set();
		m_vibroEnabledTime = systemClock->getTime();
	}
}

uint8_t IRPresentationReceiverMT2::decodeDamage(uint8_t damage)
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

void IRPresentationReceiverMT2::updateOperationCallback(OperationCallback callback)
{
	if (m_group == nullptr)
		return;
	m_group->m_operation = callback;
	m_group->m_lastReceivedMessage = systemClock->getTime();
}

// //////////////////////////
// PresentationReceiversGroupMT2

void PresentationReceiversGroupMT2::connectReceiver(IIRPresentationReceiver& receiver)
{
	m_receivers.push_back(static_cast<IRPresentationReceiverMT2*>(&receiver));
	receiver.assignReceiversGroup(*this);
}

void PresentationReceiversGroupMT2::interrogate()
{
	for (auto it = m_receivers.begin(); it != m_receivers.end(); it++)
	{
		(*it)->interrogate();
	}

	if (systemClock->getTime() - m_lastReceivedMessage > messageParsingDelay)
	{
		if (m_operation != nullptr)
		{
			m_operation();
			m_waitingOtherKillzonesShot = false;
			m_operation = nullptr;
		}
	}
}

void PresentationReceiversGroupMT2::vibrateAllZones()
{
	for (auto it = m_receivers.begin(); it != m_receivers.end(); it++)
	{
		(*it)->enableVibro();
	}
}


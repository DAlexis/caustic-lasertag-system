/*
 * ir-presentation-mt2.cpp
 *
 *  Created on: 8 янв. 2016 г.
 *      Author: alexey
 */

#include "ir/ir-presentation-mt2.hpp"
#include "ir/MT2-details.hpp"
#include "core/logging.hpp"
#include "rcsp/operation-codes.hpp"
#include "hal/system-clock.hpp"
#include "utils/memory.hpp"

FloatParameter IRPresentationReceiverMT2::defaultDamageCoefficient = 1.0;

void IRPresentationTransmitterMT2::init()
{
}

void IRPresentationTransmitterMT2::sendMessage(const RCSPStream& stream)
{
	RCSPAggregator::Operation op;
	bool success = true;
	RCSPAggregator::instance().extractNextOperation(stream.getStream(), op, stream.getSize(), success);
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

void IRPresentationReceiverMT2::init()
{

}

void IRPresentationReceiverMT2::assignReceiversGroup(IPresentationReceiversGroup& group)
{
	m_group = static_cast<PresentationReceiversGroupMT2*>(&group);
}

void IRPresentationReceiverMT2::interrogate()
{
	m_physicalReceiver->interrogate();
}

void IRPresentationReceiverMT2::setDamageCoefficient(const FloatParameter* coefficient)
{
	m_damageCoefficient = coefficient;
}

void IRPresentationReceiverMT2::receiverCallback(const uint8_t* data, uint16_t size)
{
	debug << "pres receiver has data: ";
	printHex(data, size);
	// If shot
	if (data[0] & ~MT2Extended::Byte1::shotMask == 0)
	{
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
	if (!m_group->m_hasWaitingShot || msg.damage > m_group->m_maxDamage)
	{
		m_group->m_shotMessage = msg;
		m_group->m_hasWaitingShot = true;
		updateOperationCallback(
			[this]() {
				RCSPAggregator::instance().doOperation(ConfigCodes::HeadSensor::Functions::catchShot, m_group->m_shotMessage);
			}
		);
	}
	// Doing a real call

}

void IRPresentationReceiverMT2::parseSetTeam(const uint8_t* data, uint16_t size)
{
	UNUSED_ARG(size);
	uint8_t teamId = data[1] & 0x03;
	//printf("IR: Set team id to %u\n", teamId);
	if (data[1] & ~(0x03)) {
		//printf("Warning: team id byte contains non-zero upper bits\n");
	}
	TeamMT2Id team = data[1];
	updateOperationCallback([team]() mutable {
		RCSPAggregator::instance().doOperation(
				ConfigCodes::HeadSensor::Functions::setTeam,
				team);
	});
}

void IRPresentationReceiverMT2::parseAddHealth(const uint8_t* data, uint16_t size)
{
	int16_t healthDelta = MT2Extended::decodeAddHealth(data[1]);
	updateOperationCallback([healthDelta]() mutable {
		RCSPAggregator::instance().doOperation(
				ConfigCodes::HeadSensor::Functions::addMaxHealth,
				healthDelta);
	});
}

void IRPresentationReceiverMT2::parseMT2Command(const uint8_t* data, uint16_t size)
{
	switch(data[1])
	{
	case MT2Extended::Commands::adminKill:
		updateOperationCallback([]() { RCSPAggregator::instance().doOperation(ConfigCodes::HeadSensor::Functions::playerKill); });
		break;
	case MT2Extended::Commands::pauseOrUnpause:
		break;
	case MT2Extended::Commands::startGame:
		updateOperationCallback([]() { RCSPAggregator::instance().doOperation(ConfigCodes::HeadSensor::Functions::playerReset); });
		break;
	case MT2Extended::Commands::restoreDefaults:
		updateOperationCallback([]() { RCSPAggregator::instance().doOperation(ConfigCodes::AnyDevice::Functions::resetToDefaults); });
		break;
	case MT2Extended::Commands::respawn:
		updateOperationCallback([]() { RCSPAggregator::instance().doOperation(ConfigCodes::HeadSensor::Functions::playerRespawn); });
		break;
	case MT2Extended::Commands::newGameImmediate:
		break;
	case MT2Extended::Commands::fullAmmo:
		break;
	case MT2Extended::Commands::endGame:
		break;
	case MT2Extended::Commands::resetClock:
		break;
	case MT2Extended::Commands::initializePlayer:
		break;
	case MT2Extended::Commands::explodePlayer:
		break;
	case MT2Extended::Commands::newGameReady:
		break;
	case MT2Extended::Commands::fullHealth:
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
	if (!RCSPAggregator::instance().isStreamConsistent(stream, streamSize))
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
		RCSPAggregator::instance().dispatchStream(m_group->m_bufferForArgument, streamSize);
	});
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

	Time time = systemClock->getTime();
	if (time - m_lastReceivedMessage > messageParsingDelay)
	{
		if (m_operation != nullptr)
		{
			m_operation();
			m_hasWaitingShot = false;
			m_operation = nullptr;
		}
	}
}


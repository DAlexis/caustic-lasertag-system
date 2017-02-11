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

#include "sensors/ir-protocol-parser-MT2-ex.hpp"
#include "ir/MT2-details.hpp"
#include "core/logging.hpp"
#include "rcsp/operation-codes.hpp"

IRProtocolParserMilesTag2Ex::IRProtocolParserMilesTag2Ex(RCSPAggregator& rcspAggregator, IlluminationSchemesManager& schemes) :
	m_rcspAggregator(rcspAggregator),
	m_schemes(schemes)
{
}

void IRProtocolParserMilesTag2Ex::parse(IRProtocolParseResult& result, uint8_t* data, uint16_t bitsCount, UintParameter sensorId)
{
	UNUSED_ARG(sensorId);
	// If shot
	if ((data[0] & ~MT2Extended::Byte1::shotMask) == 0)
	{
		// For compatibility with protocol extensions no limitations to package size are provided
		parseShot(result, data, bitsCount);
		return;
	}
	// Parsing a "command"
	if (bitsCount == MT2Extended::commandLength
		&& data[0] == MT2Extended::Byte1::command
		&& data[2] == MT2Extended::Byte3::commandEnd)
	{
		parseMT2Command(result, data, bitsCount);
		return;
	}

	// Team change
	if (bitsCount == MT2Extended::messageLength
		&& data[0] == MT2Extended::Byte1::setTeam)
	{
		parseSetTeam(result, data, bitsCount);
		return;
	}

	// Health count changing
	if (bitsCount == MT2Extended::messageLength
		&& data[0] == MT2Extended::Byte1::addHealth)
	{
		parseAddHealth(result, data, bitsCount);
		return;
	}

	if (bitsCount > 8 + RCSPAggregator::minimalStreamSize
		&& bitsCount % 8 == 0
		&& data[0] == MT2Extended::Byte1::RCSPMessage)

	{
		parseRCSP(result, data, bitsCount);
		return;
	}
}

void IRProtocolParserMilesTag2Ex::parseShot(IRProtocolParseResult& result, const uint8_t* data, uint16_t size)
{
	UNUSED_ARG(size);
	if (size < MT2Extended::shotLength)
	{
		warning << "Shot package with len < " << size << " ignored";
		result.type = IRProtocolParseResult::Type::invalid;
		return;
	}
	result.type = IRProtocolParseResult::Type::shot;
	result.shot.playerId  = data[0] & ~(1 << 7);
	result.shot.teamId    = data[1] >> 6;
	unsigned int damageCode = (data[1] & 0b00111100) >> 2;
	result.shot.damage = decodeDamage(damageCode);
}

void IRProtocolParserMilesTag2Ex::parseSetTeam(IRProtocolParseResult& result, const uint8_t* data, uint16_t size)
{
	UNUSED_ARG(size);
	if (data[1] & ~(0x03)) {
		warning << "Team id byte contains non-zero upper bits";
	}
	result.type = IRProtocolParseResult::Type::command;
	TeamGameId team = data[1] & 0x03;
	result.commandCallback = [team, this]() mutable {
		m_rcspAggregator.doOperation(
				ConfigCodes::HeadSensor::Functions::setTeam,
				team);
	};
}

void IRProtocolParserMilesTag2Ex::parseAddHealth(IRProtocolParseResult& result, const uint8_t* data, uint16_t size)
{
	UNUSED_ARG(size);
	int16_t healthDelta = MT2Extended::decodeAddHealth(data[1]);
	result.type = IRProtocolParseResult::Type::command;
	result.commandCallback = [healthDelta, this]() mutable {
		m_rcspAggregator.doOperation(
				ConfigCodes::HeadSensor::Functions::addMaxHealth,
				healthDelta);
	};
}

void IRProtocolParserMilesTag2Ex::parseMT2Command(IRProtocolParseResult& result, const uint8_t* data, uint16_t size)
{
	UNUSED_ARG(size);
	result.type = IRProtocolParseResult::Type::command;
	switch(data[1])
	{
	case MT2Extended::Commands::adminKill:
		result.commandCallback = [this]() { m_rcspAggregator.doOperation(ConfigCodes::HeadSensor::Functions::playerKill); };
		break;
	case MT2Extended::Commands::pauseOrUnpause:
		break;
	case MT2Extended::Commands::startGame:
		result.commandCallback = [this]() { m_rcspAggregator.doOperation(ConfigCodes::HeadSensor::Functions::playerReset); };
		break;
	case MT2Extended::Commands::restoreDefaults:
		result.commandCallback = [this]() { m_rcspAggregator.doOperation(ConfigCodes::AnyDevice::Functions::resetToDefaults); };
		break;
	case MT2Extended::Commands::fullHealth:
	case MT2Extended::Commands::initializePlayer:
	case MT2Extended::Commands::newGameReady:
	case MT2Extended::Commands::newGameImmediate:
	case MT2Extended::Commands::respawn:
		result.commandCallback = [this]() { m_rcspAggregator.doOperation(ConfigCodes::HeadSensor::Functions::playerRespawn); };
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
		result.type = IRProtocolParseResult::Type::invalid;
		return;
	}
}

void IRProtocolParserMilesTag2Ex::parseRCSP(IRProtocolParseResult& result, const uint8_t* data, uint16_t size)
{
	const uint8_t* stream = &data[1];
	uint16_t streamSize = size-1;
	if (!m_rcspAggregator.isStreamConsistent(stream, streamSize))
	{
		warning << "Inconsistent RCSP stream on IR channel";
		return;
	}
	debug << "General RCSP message by IR receiver";
	if (size > IRProtocolParserMilesTag2Ex::argumentBufferSize)
	{
		error << "RCSP IR stream is too large to be copied to temporary buffer";
		return;
	}
	memcpy(m_bufferForArgument, stream, streamSize);
	result.type = IRProtocolParseResult::Type::command;
	result.commandCallback = [this, streamSize]() mutable {
		m_rcspAggregator.dispatchStream(m_bufferForArgument, streamSize);
	};
}

uint8_t IRProtocolParserMilesTag2Ex::decodeDamage(uint8_t damage)
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

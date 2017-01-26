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

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PROTOCOL_PARSER_MT2_EX_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PROTOCOL_PARSER_MT2_EX_HPP_

#include "ir2/ir-presentation-receiver.hpp"
#include "rcsp/aggregator.hpp"

class IRProtocolParserMilesTag2Ex : public IIRProtocolParser
{
public:
	IRProtocolParserMilesTag2Ex(RCSPAggregator& rcspAggregator);
	void parse(IRProtocolParseResult& result, uint8_t* data, uint16_t bitsCount, UintParameter sensorId);

private:
	constexpr static uint16_t argumentBufferSize = 100;
	void parseShot(IRProtocolParseResult& result, const uint8_t* data, uint16_t size);
	void parseMT2Command(IRProtocolParseResult& result, const uint8_t* data, uint16_t size);
	void parseSetTeam(IRProtocolParseResult& result, const uint8_t* data, uint16_t size);
	void parseAddHealth(IRProtocolParseResult& result, const uint8_t* data, uint16_t size);
	void parseRCSP(IRProtocolParseResult& result, const uint8_t* data, uint16_t size);
	uint8_t decodeDamage(uint8_t damage);

	uint8_t m_bufferForArgument[argumentBufferSize];

	RCSPAggregator& m_rcspAggregator;
};





#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_IR_PROTOCOL_PARSER_MT2_EX_HPP_ */

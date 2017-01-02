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


#include "rcsp/aggregator.hpp"
#include "rcsp/state-saver.hpp"
#include "rcsp/stream.hpp"
#include "core/string-utils.hpp"
#include "core/logging.hpp"
#include "fatfs.h"
#include <stdio.h>

RCSPAggregator* RCSPAggregator::m_activeAggregator = nullptr;

RCSPAggregator& RCSPAggregator::getActiveAggregator()
{
    return *m_activeAggregator;
}

void RCSPAggregator::setActiveAggregator(RCSPAggregator* aggregator)
{
    m_activeAggregator = aggregator;
}

void RCSPAggregator::registerAccessor(OperationCode code, const char* textName, IOperationAccessor* accessor, bool restorable)
{
	m_accessorsByOpCode[code] = accessor;
	m_accessorsByOpText[textName] = accessor;
	if (restorable)
	{
		if (!RCSPCodeManipulator::isSetObject(code))
		{
			error << "Only parameter\'s values can save states!";
			return;
		}
		m_restorable.push_back(code);
	}
}

uint32_t RCSPAggregator::dispatchStream(uint8_t* stream, uint32_t size, RCSPMultiStream* answerStream)
{
	uint8_t* position = stream;
	uint32_t unsupported = 0;
	while ( (uint32_t) (position - stream) <= size - (sizeof(OperationSize)+sizeof(OperationCode)) )
	{
		OperationSize *pOperationSize = reinterpret_cast<OperationSize*> (position);
		position += sizeof(OperationSize);
		OperationCode *pOperationCode = reinterpret_cast<OperationCode*> (position);
		position += sizeof(OperationCode);
		if (!dispatchOperation(pOperationSize, pOperationCode, position, answerStream))
			unsupported++;
		position += *pOperationSize;
	}
	return unsupported;
}

const uint8_t* RCSPAggregator::extractNextOperation(
			const uint8_t* stream,
			RCSPAggregator::Operation& commad,
			uint16_t streamSize,
			bool& success
		)
{
	OperationSize size;
	deserializeAndInc(stream, size);
	if (streamSize < size + sizeof(OperationSize)+sizeof(OperationCode))
	{
		success = false;
		return stream;
	}
	OperationCode code;
	deserializeAndInc(stream, code);
	commad.argumentSize = size;
	commad.code = code;
	commad.argument = stream;
	stream += size;
	success = true;
	return stream;
}

bool RCSPAggregator::dispatchOperation(OperationSize* size, OperationCode* code, uint8_t* arg, RCSPMultiStream* answerStream)
{
	if (RCSPCodeManipulator::isObjectRequest(*code))
	{
		if (answerStream)
		{
			// Adding parameter to answer stream
			OperationCode parameterCode = RCSPCodeManipulator::makeSetObject(*code);
			//printf("Parameter request: %u\n", parameterCode);
			auto it = m_accessorsByOpCode.find(parameterCode);
			if (it != m_accessorsByOpCode.end())
			{
				answerStream->addValue(parameterCode);
				return true;
			} else {
				printWarningUnknownCode(*code);
				return false;
			}
		} else {
			debug << "No answer stream, skipping request";
			return false;
		}
	} else {
		auto it = m_accessorsByOpCode.find(*code);
		if (it != m_accessorsByOpCode.end())
		{
			trace << "Dispatched opcode: " << *code ;
			if (*size == 0)
				it->second->deserialize(nullptr, 0);
			else
				it->second->deserialize(arg, *size);
			return true;
		}
		else
		{
			printWarningUnknownCode(*code);
			return false;
		}
	}
}

void RCSPAggregator::printWarningUnknownCode(OperationCode code)
{
	//if (code != ConfigCodes::noOperation)
	if (code != 0)
		warning << "Unknown request code: " << code;
}


bool RCSPAggregator::isStreamConsistent(const uint8_t* stream, uint32_t size)
{
	for (const uint8_t* position = stream; (uint32_t) (position - stream) <= size; position++)
	{
		OperationSize operationSize = *position;
		unsigned int thisOperationSize = sizeof(operationSize)+sizeof(OperationCode)+operationSize;
		position += thisOperationSize;
		/// TODO Add "nope" ignoring support
		if ((uint32_t) (position - stream) == size)
			return true;
		else if ((uint32_t) (position - stream) > size)
			return false;
	}
	return false;
}


Result RCSPAggregator::parseSring(const char* key, const char* value)
{
	//printf("Parsing k = %s, v = %s\n", key, value);
	auto it = m_accessorsByOpText.find(key);
	if (it == m_accessorsByOpText.end())
	{
		warning << "Unknown variable: " << key;
		return Result("Unknown variable");
	}
	if (!it->second->isWritable())
	{
		return Result("Parameter cannot be set\n");
	}

	it->second->parseString(value);
	return Result();
}

DetailedResult<RCSPAggregator::AddingResult> RCSPAggregator::serializeObject(
		uint8_t* stream,
		OperationCode code,
		uint16_t freeSpace,
		uint16_t& actualSize,
		const uint8_t* pCustomValue)
{
	actualSize = 0;
	auto it = m_accessorsByOpCode.find(code);
	if (it == m_accessorsByOpCode.end())
	{
		return DetailedResult<AddingResult>(INVALID_OPCODE, "Opcode not found");
	}
	code = RCSPCodeManipulator::makeSetObject(code);
	OperationSize size = 0;
	if (!it->second->isReadable())
		return DetailedResult<AddingResult>(NOT_READABLE, "Object not readable");

	size = it->second->getSize();
	uint16_t packageSize = size + sizeof(OperationSize) + sizeof(OperationCode);
	if (freeSpace < packageSize)
	{
		return DetailedResult<AddingResult>(NOT_ENOUGH_SPACE, "Not enough space in stream");
	}
	memcpy(stream, &size, sizeof(OperationSize));
	stream += sizeof(OperationSize);
	memcpy(stream, &code, sizeof(OperationCode));
	stream += sizeof(OperationCode);
	if (pCustomValue == nullptr)
	{
		it->second->serialize(stream);
	} else {
		memcpy(stream, pCustomValue, size);
	}

	actualSize = sizeof(OperationSize) + sizeof(OperationCode) + size;
	return DetailedResult<AddingResult>(OK);
}

DetailedResult<RCSPAggregator::AddingResult> RCSPAggregator::serializeObjectRequest(uint8_t* stream, OperationCode variableCode, uint16_t freeSpace, uint16_t& actualSize)
{
	actualSize = 0;
	uint16_t packageSize = sizeof(OperationSize) + sizeof(OperationCode);
	if (packageSize > freeSpace)
		return DetailedResult<AddingResult>(NOT_ENOUGH_SPACE, "Not enough space in stream");

	variableCode = RCSPCodeManipulator::makeObjectRequestOC(variableCode);
	OperationSize size = 0;
	memcpy(stream, &size, sizeof(OperationSize));
	stream += sizeof(OperationSize);
	memcpy(stream, &variableCode, sizeof(OperationCode));
	actualSize = packageSize;
	return DetailedResult<AddingResult>(OK);
}

DetailedResult<RCSPAggregator::AddingResult> RCSPAggregator::serializeCallRequest(uint8_t* stream, OperationCode functionCode, uint16_t freeSpace, uint16_t& actualSize)
{
	actualSize = 0;
	uint16_t packageSize = sizeof(OperationSize) + sizeof(OperationCode);
	if (packageSize > freeSpace)
		return DetailedResult<AddingResult>(NOT_ENOUGH_SPACE, "Not enough space in stream");

	functionCode = RCSPCodeManipulator::makeCallRequest(functionCode);
	OperationSize size = 0;
	memcpy(stream, &size, sizeof(OperationSize));
	stream += sizeof(OperationSize);
	memcpy(stream, &functionCode, sizeof(OperationCode));
	actualSize = packageSize;
	return DetailedResult<AddingResult>(OK);
}

Result RCSPAggregator::readIni(const char* filename)
{
	IniParser* parcer = new IniParser;
	//parcer->setCallback([this](const char* key, const char* value){ parseSring(key, value); });
	parcer->setCallback(std::bind(&RCSPAggregator::parseSring, this, std::placeholders::_1, std::placeholders::_2));
	Result res = parcer->parseFile(filename);
	delete parcer;
	return res;

}

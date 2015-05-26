/*
 * configuration.cpp
 *
 *  Created on: 10 янв. 2015 г.
 *      Author: alexey
 */

#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "rcsp/RCSP-state-saver.hpp"
#include "core/string-utils.hpp"
#include "core/logging.hpp"
#include "hal/ff/ff.h"
#include <stdio.h>

RCSPAggregator* RCSPAggregator::m_RCSPAggregator = nullptr;

STATIC_DEINITIALIZER_IN_CPP_FILE(RCSPAggregator, m_RCSPAggregator)

RCSPAggregator& RCSPAggregator::instance()
{
	if (!m_RCSPAggregator)
		m_RCSPAggregator = new RCSPAggregator;
	return *m_RCSPAggregator;
}

void RCSPAggregator::registerAccessor(OperationCode code, const char* textName, IOperationAccessor* accessor, bool restorable)
{
	ScopedTag tag("reg-accessor");
	//printf("Accepted %u: %s\n", code - (1<<14), textName);
	m_accessorsByOpCode[code] = accessor;
	m_accessorsByOpText[textName] = accessor;
	if (restorable)
	{
		if (!isObjectOC(code))
		{
			error << "Only parameter\'s values can save states!\n";
			return;
		}
		StateSaver::instance().addValue(code);
	}
}

uint32_t RCSPAggregator::dispatchStream(uint8_t* stream, uint32_t size, RCSPMultiStream* answerStream)
{
	uint8_t* position = stream;
	uint32_t unsupported = 0;
	while ((position - stream) <= size - (sizeof(OperationSize)+sizeof(OperationCode)) )
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

bool RCSPAggregator::dispatchOperation(OperationSize* size, OperationCode* code, uint8_t* arg, RCSPMultiStream* answerStream)
{
	ScopedTag tag("dispatch-op");
	if (isObjectRequestOC(*code))
	{
		if (answerStream)
		{
			// Adding parameter to answer stream
			OperationCode parameterCode = SetObjectOC(*code);
			//printf("Parameter request: %u\n", parameterCode);
			auto it = m_accessorsByOpCode.find(parameterCode);
			if (it != m_accessorsByOpCode.end())
			{
				answerStream->addValue(parameterCode);
				return true;
			} else {
				warning << "Unknown request code: " << *code << "\n";
				return false;
			}
		} else {
			debug << "No answer stream, skipping request\n";
			return false;
		}
	} else {
		auto it = m_accessorsByOpCode.find(*code);
		if (it != m_accessorsByOpCode.end())
		{
			trace << "Dispatched opcode: " << *code << "\n";
			if (*size == 0)
				it->second->deserialize(nullptr, 0);
			else
				it->second->deserialize(arg, *size);
			return true;
		}
		else
			return false;
	}
}


bool RCSPAggregator::isStreamConsistent(uint8_t* stream, uint32_t size)
{
	//uint8_t* position = stream;
	for (uint8_t* position = stream; position - stream <= size; position++)
	{
		OperationSize operationSize = *position;
		unsigned int thisOperationSize = sizeof(operationSize)+sizeof(OperationCode)+operationSize;
		position += thisOperationSize;
		/// TODO Add "nope" ignoring support
		if (position - stream == size)
			return true;
		else if (position - stream > size)
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
		printf("Unknown variable\n");
		return Result("Unknown variable");
	}
	if (!it->second->isWritable())
	{
		return Result("Parameter cannot be set\n");
	}

	it->second->parseString(value);
	return Result();
}

DetailedResult<RCSPAggregator::AddingResult> RCSPAggregator::serializeObject(uint8_t* stream, OperationCode code, uint16_t freeSpace, uint16_t& actualSize)
{
	actualSize = 0;
	auto it = m_accessorsByOpCode.find(code);
	if (it == m_accessorsByOpCode.end())
	{
		return DetailedResult<AddingResult>(INVALID_OPCODE, "Opcode not found");
	}
	code = SetObjectOC(code);
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
	it->second->serialize(stream);

	actualSize = sizeof(OperationSize) + sizeof(OperationCode) + size;
	return DetailedResult<AddingResult>(OK);
}

DetailedResult<RCSPAggregator::AddingResult> RCSPAggregator::serializeObjectRequest(uint8_t* stream, OperationCode variableCode, uint16_t freeSpace, uint16_t& actualSize)
{
	actualSize = 0;
	uint16_t packageSize = sizeof(OperationSize) + sizeof(OperationCode);
	if (packageSize > freeSpace)
		return DetailedResult<AddingResult>(NOT_ENOUGH_SPACE, "Not enough space in stream");

	variableCode = SetObjectRequestOC(variableCode);
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

	functionCode = SetCallRequestOC(functionCode);
	OperationSize size = 0;
	memcpy(stream, &size, sizeof(OperationSize));
	stream += sizeof(OperationSize);
	memcpy(stream, &functionCode, sizeof(OperationCode));
	actualSize = packageSize;
	return DetailedResult<AddingResult>(OK);
}

Result RCSPAggregator::readIni(const char* filename)
{
	IniParcer* parcer = new IniParcer;
	//parcer->setCallback([this](const char* key, const char* value){ parseSring(key, value); });
	parcer->setCallback(std::bind(&RCSPAggregator::parseSring, this, std::placeholders::_1, std::placeholders::_2));
	Result res = parcer->parseFile(filename);
	delete parcer;
	return res;

}

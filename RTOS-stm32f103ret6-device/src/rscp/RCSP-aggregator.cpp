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
#include "fatfs.h"
#include <stdio.h>

RCSPAggregator* RCSPAggregator::m_RCSPAggregator = nullptr;

SINGLETON_IN_CPP(RCSPAggregator)

void RCSPAggregator::registerAccessor(OperationCode code, const char* textName, IOperationAccessor* accessor, bool restorable)
{
	ScopedTag tag("reg-accessor");
	m_accessorsByOpCode[code] = accessor;
	m_accessorsByOpText[textName] = accessor;
	if (restorable)
	{
		if (!RCSPCodeManipulator::isSetObject(code))
		{
			error << "Only parameter\'s values can save states!";
			return;
		}
		MainStateSaver::instance().addValue(code);
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
	ScopedTag tag("dispatch-op");
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
	//uint8_t* position = stream;
	for (const uint8_t* position = stream; position - stream <= size; position++)
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
	IniParcer* parcer = new IniParcer;
	//parcer->setCallback([this](const char* key, const char* value){ parseSring(key, value); });
	parcer->setCallback(std::bind(&RCSPAggregator::parseSring, this, std::placeholders::_1, std::placeholders::_2));
	Result res = parcer->parseFile(filename);
	delete parcer;
	return res;

}

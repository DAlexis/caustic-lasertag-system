/*
 * configuration.cpp
 *
 *  Created on: 10 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/configuration.hpp"
#include "core/string-utils.hpp"
#include "hal/ff/ff.h"
#include <memory>
#include <stdio.h>

ConfigsAggregator* ConfigsAggregator::m_configsAggregator = nullptr;

STATIC_DEINITIALIZER_IN_CPP_FILE(ConfigsAggregator, m_configsAggregator)

ConfigsAggregator& ConfigsAggregator::instance()
{
	if (!m_configsAggregator)
		m_configsAggregator = new ConfigsAggregator;
	return *m_configsAggregator;
}

void ConfigsAggregator::registerAccessor(OperationCode code, const char* textName, IOperationAccessor* accessor)
{
	//printf("Accepted %u: %s\n", code - (1<<14), textName);
	m_accessorsByOpCode[code] = accessor;
	m_accessorsByOpText[textName] = accessor;
}

uint32_t ConfigsAggregator::dispatchStream(uint8_t* stream, uint32_t size, StreamGenerator* answerStream)
{
	uint8_t* position = stream;
	uint32_t unsupported = 0;
	while ((position - stream) <= size-(sizeof(OperationSize)+sizeof(OperationCode)))
	{
		OperationSize *pOperationSize = reinterpret_cast<OperationSize*> (position);
		position += sizeof(OperationSize);
		OperationCode *pOperationCode = reinterpret_cast<OperationCode*> (position);
		position += sizeof(OperationCode);

		if (isParameterRequest(*pOperationCode))
		{
			if (answerStream)
			{
				// Adding parameter to answer stream
				OperationCode parameterCode = SetParameterOC(*pOperationCode);
				printf("Parameter request: %u\n", parameterCode);
				auto it = m_accessorsByOpCode.find(*pOperationCode);
				if (it != m_accessorsByOpCode.end())
				{
					answerStream->add(parameterCode);
				}
			}
		} else {
			auto it = m_accessorsByOpCode.find(*pOperationCode);
			if (it != m_accessorsByOpCode.end())
			{
				printf("Dispatched opcode: %u\n", *pOperationCode);
				it->second->deserialize(position, *pOperationSize);
			}
			else
				unsupported++;
		}
		position += *pOperationSize;
	}
	return unsupported;
}

Result ConfigsAggregator::parseSring(const char* key, const char* value)
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
		return Result("Parameter cannot be setted");
	}

	it->second->parseString(value);
	return Result();
}

uint16_t ConfigsAggregator::serialize(uint8_t* stream, OperationCode code, uint16_t maxSize)
{
	auto it = m_accessorsByOpCode.find(code);
	if (it == m_accessorsByOpCode.end())
		return 0;
	/// @todo Add two return variants: maxSize reached and code not found
	OperationSize size = 0;
	if (it->second->isReadable())
	{
		// We have readable object
		size = it->second->getSize();
		uint16_t packageSize = size + sizeof(OperationSize) + sizeof(OperationCode);
		if (maxSize < packageSize)
			return 0;
		memcpy(stream, &size, sizeof(OperationSize));
		stream += sizeof(OperationSize);
		memcpy(stream, &code, sizeof(OperationCode));
		stream += sizeof(OperationCode);
		it->second->serialize(stream);
	} else {
		// We have unreadable object
		size = 0;
		uint16_t packageSize = sizeof(OperationSize) + sizeof(OperationCode);
		if (maxSize < packageSize)
			return 0;
		memcpy(stream, &size, sizeof(OperationSize));
		stream += sizeof(OperationSize);
		memcpy(stream, &code, sizeof(OperationCode));
	}
	return sizeof(OperationSize) + sizeof(OperationCode) + size;
}

uint16_t ConfigsAggregator::serializeWithoutArgumentLookup(uint8_t* stream, OperationCode code, uint16_t maxSize)
{
	uint16_t packageSize = sizeof(OperationSize) + sizeof(OperationCode);
	if (packageSize > maxSize)
		return 0;

	OperationSize size = 0;
	memcpy(stream, &size, sizeof(OperationSize));
	stream += sizeof(OperationSize);
	memcpy(stream, &code, sizeof(OperationCode));
	return packageSize;
}

Result ConfigsAggregator::readIni(const char* filename)
{
	IniParcer* parcer = new IniParcer;
	//parcer->setCallback([this](const char* key, const char* value){ parseSring(key, value); });
	parcer->setCallback(std::bind(&ConfigsAggregator::parseSring, this, std::placeholders::_1, std::placeholders::_2));
	Result res = parcer->parseFile(filename);
	delete parcer;
	return res;

}



StreamGenerator::StreamGenerator(uint16_t size) :
	m_size(size)
{
	m_stream = new uint8_t[m_size];
	memset(m_stream, 0, size*sizeof(uint8_t));
}

StreamGenerator::~StreamGenerator()
{
	if (m_stream)
		delete[] m_stream;
}

uint8_t* StreamGenerator::getStream()
{
	return m_stream;
}

uint16_t StreamGenerator::getSize()
{
	return m_size;
}

bool StreamGenerator::add(OperationCode code, bool needArgumentLookup)
{
	printf("Adding to stream code %u\n", code);
	uint8_t *pos = m_stream + m_cursor;

	uint16_t addedSize = needArgumentLookup ?
			ConfigsAggregator::instance().serialize(pos, code, m_size - m_cursor)
			: ConfigsAggregator::instance().serializeWithoutArgumentLookup(pos, code, m_size - m_cursor);

	if (addedSize != 0)
	{
		printf("Added to stream opcode %u\n", code);
		m_cursor += addedSize;
		return true;
	} else {
		printf("Warning: no more size in stream to add opcode %u!\n", code);
		return false;
	}
}

bool StreamGenerator::empty()
{
	return (m_cursor == 0);
}

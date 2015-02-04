/*
 * configuration.cpp
 *
 *  Created on: 10 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/configuration.hpp"
#include "core/string-utils.hpp"
#include "hal/ff/ff.h"
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

uint32_t ConfigsAggregator::dispatchStream(uint8_t* stream, uint32_t size)
{
	uint8_t* position = stream;
	uint32_t unsupported = 0;
	while ((position - stream) <= size-(sizeof(OperationSize)+sizeof(OperationCode)))
	{
		OperationSize *pOperationSize = reinterpret_cast<OperationSize*> (position);
		position += sizeof(OperationSize);
		OperationCode *pOperationCode = reinterpret_cast<OperationCode*> (position);
		position += sizeof(OperationCode);
		auto it = m_accessorsByOpCode.find(*pOperationCode);
		if (it != m_accessorsByOpCode.end())
		{
			printf("Dispatched opcode %u\n", *pOperationCode);
			it->second->deserialize(position, *pOperationSize);
		}
		else
			unsupported++;
		position += *pOperationSize;
	}
	return unsupported;
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

void ConfigsAggregator::readFromFile(const char* filename)
{
	constexpr uint32_t bufferSize = 512;

	FRESULT res;
	FIL fil;
	res = f_open(&fil, filename, FA_OPEN_EXISTING | FA_READ);

	UINT readed=0;
	uint8_t *buffer = new uint8_t[bufferSize];

	bool isCommentNow = false;
	while (not f_eof(&fil))
	{
		res = f_read(&fil, buffer, bufferSize, &readed);
		if (res != FR_OK)
			return;

		uint16_t cursor = 0;

		if (isCommentNow)
		{
			while (cursor < readed && buffer[cursor] != '\n')
				cursor++;

			// If no comment's end in this block
			if (cursor == readed)
				continue;
			// If we are here, comment was skipped
			isCommentNow = false;
		}

		// Skipping spaces
		while (cursor < readed && isSpace(buffer[cursor]))
			cursor++;

		// If all symbols are spaces
		if (cursor == readed)
			continue;

		// If comment
		if (buffer[cursor] == '#')
		{
			// Looking for eol
			while (cursor < readed && buffer[cursor] != '\n')
				cursor++;
			// If comments are longer then buffer
			if (cursor == readed)
			{
				isCommentNow = true;
				continue;
			}
		}

		// If we are here, we have probably beginning of token
	}

	delete[] buffer;
	f_close(&fil);
}



StreamGenerator::StreamGenerator(uint16_t size) :
	m_size(size)
{
	m_stream = new uint8_t[size];
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

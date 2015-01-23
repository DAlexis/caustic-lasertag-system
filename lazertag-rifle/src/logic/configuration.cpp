/*
 * configuration.cpp
 *
 *  Created on: 10 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/configuration.hpp"

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
	while ((position - stream) < size)
	{
		OperationSize *pOperationSize = reinterpret_cast<OperationSize*> (position);
		position += sizeof(OperationSize);
		OperationCode *pOperationCode = reinterpret_cast<OperationCode*> (position);
		position += sizeof(OperationCode);
		auto it = m_accessorsByOpCode.find(*pOperationCode);
		if (it != m_accessorsByOpCode.end())
			it->second->deserialize(position, *pOperationSize);
		else
			unsupported++;
		position += *pOperationSize;
	}
	return unsupported;
}


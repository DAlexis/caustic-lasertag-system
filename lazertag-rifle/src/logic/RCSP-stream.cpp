/*
 * RCSP-stream.cpp
 *
 *  Created on: 11 февр. 2015 г.
 *      Author: alexey
 */

#include "logic/RCSP-aggregator.hpp"
#include "logic/RCSP-stream.hpp"
#include "core/string-utils.hpp"
#include "hal/ff/ff.h"
#include <stdio.h>


RCSPStream::RCSPStream(uint16_t size) :
	m_size(size)
{
	m_stream = new uint8_t[m_size];
	m_freeSpace = m_size;
	memset(m_stream, 0, size*sizeof(uint8_t));
}

RCSPStream::~RCSPStream()
{
	if (m_stream)
		delete[] m_stream;
}

uint8_t* RCSPStream::getStream()
{
	return m_stream;
}

uint16_t RCSPStream::getSize()
{
	return m_size;
}

bool RCSPStream::addValue(OperationCode code)
{
	printf("Adding value, code %u", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return RCSPAggregator::instance().serializeObject(pos, code, m_size - m_cursor, addedSize);
		}
	);
}

bool RCSPStream::addRequest(OperationCode code)
{
	printf("Adding value, code %u", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return RCSPAggregator::instance().requestVariable(pos, code, m_size - m_cursor, addedSize);
		}
	);
}

bool RCSPStream::addCall(OperationCode code)
{
	printf("Adding value, code %u", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return RCSPAggregator::instance().functionCall(pos, code, m_size - m_cursor, addedSize);
		}
	);
}

bool RCSPStream::serializeAnything(OperationCode code, SerializationFunc serializer)
{
	uint8_t *pos = m_stream + m_cursor;
	uint16_t addedSize = 0;
	RCSPAggregator::ResultType result = serializer(pos, code, addedSize);
	if (!result.isSuccess)
	{
		printf("Cannot add %u: %s\n", code, result.errorText);
		return false;
	}
	m_cursor += addedSize;
	return true;
}

uint16_t RCSPStream::send(DeviceAddress target, bool waitForAck, PackageSendingDoneCallback doneCallback)
{
	return RCSPModem::instance().send(target, m_stream, m_size, waitForAck, doneCallback);
}

bool RCSPStream::empty()
{
	return (m_cursor == 0);
}

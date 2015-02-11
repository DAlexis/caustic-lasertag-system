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

RCSPAggregator::ResultType RCSPStream::addValue(OperationCode code)
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

RCSPAggregator::ResultType RCSPStream::addRequest(OperationCode code)
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

RCSPAggregator::ResultType RCSPStream::addCall(OperationCode code)
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

RCSPAggregator::ResultType RCSPStream::serializeAnything(OperationCode code, SerializationFunc serializer)
{
	uint8_t *pos = m_stream + m_cursor;
	uint16_t addedSize = 0;
	RCSPAggregator::ResultType result = serializer(pos, code, addedSize);
	if (!result.isSuccess)
	{
		printf("Cannot add %u: %s\n", code, result.errorText);
	} else {
		m_cursor += addedSize;
	}
	return result;
}

uint16_t RCSPStream::send(DeviceAddress target, bool waitForAck, PackageSendingDoneCallback doneCallback)
{
	return RCSPModem::instance().send(target, m_stream, m_size, waitForAck, doneCallback);
}

uint16_t RCSPStream::send(
	DeviceAddress target,
	bool waitForAck,
	PackageSendingDoneCallback doneCallback,
	uint32_t timeout,
	uint32_t resendTime,
	uint32_t resendTimeDelta
)
{
	return RCSPModem::instance().send(
		target,
		m_stream,
		m_size,
		waitForAck,
		doneCallback,
		timeout,
		resendTime,
		resendTimeDelta
	);
}

bool RCSPStream::empty()
{
	return (m_cursor == 0);
}

//////////////////////////
// RCSPMultiStream

RCSPMultiStream::RCSPMultiStream()
{
	pushBackStream();
}

void RCSPMultiStream::pushBackStream()
{
	printf("Creating new stream\n");
	m_streams.push_back(std::shared_ptr<RCSPStream> (new RCSPStream));
}

RCSPAggregator::ResultType RCSPMultiStream::addValue(OperationCode code)
{
	RCSPAggregator::ResultType result = m_streams.back()->addValue(code);
	if (!result.isSuccess && result.details == RCSPAggregator::NOT_ENOUGH_SPACE)
	{
		pushBackStream();
		result = m_streams.back()->addValue(code);
	}
	return result;
}

RCSPAggregator::ResultType RCSPMultiStream::addRequest(OperationCode code)
{
	RCSPAggregator::ResultType result = m_streams.back()->addRequest(code);
	if (!result.isSuccess && result.details == RCSPAggregator::NOT_ENOUGH_SPACE)
	{
		pushBackStream();
		result = m_streams.back()->addRequest(code);
	}
	return result;
}

RCSPAggregator::ResultType RCSPMultiStream::addCall(OperationCode code)
{
	RCSPAggregator::ResultType result = m_streams.back()->addCall(code);
	if (!result.isSuccess && result.details == RCSPAggregator::NOT_ENOUGH_SPACE)
	{
		pushBackStream();
		result = m_streams.back()->addCall(code);
	}
	return result;
}

void RCSPMultiStream::send(
		DeviceAddress target,
		bool waitForAck
	)
{
	for (auto it=m_streams.begin(); it != m_streams.end(); it++)
	{
		(*it)->send(target, waitForAck);
	}
}

void RCSPMultiStream::send(
		DeviceAddress target,
		bool waitForAck,
		uint32_t timeout,
		uint32_t resendTime,
		uint32_t resendTimeDelta
	)
{
	for (auto it=m_streams.begin(); it != m_streams.end(); it++)
	{
		(*it)->send(target, waitForAck, nullptr, timeout, resendTime, resendTimeDelta);
	}
}

bool RCSPMultiStream::empty()
{
	return m_streams.front()->empty();
}

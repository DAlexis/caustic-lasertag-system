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

#include "rcsp/stream.hpp"
#include "core/string-utils.hpp"
#include "core/logging.hpp"
#include "fatfs.h"
#include <stdio.h>

RCSPStreamNew::RCSPStreamNew(RCSPAggregator* aggregator) :
	m_aggregator(aggregator)
{
}

RCSPAggregator::ResultType RCSPStreamNew::serializeAnything(OperationCode code, SerializationFunc serializer)
{/*
	uint8_t *pos = m_stream + m_cursor;
	uint16_t addedSize = 0;
	RCSPAggregator::ResultType result = serializer(pos, code, addedSize);
	if (result.isSuccess)
	{
		m_cursor += addedSize;
	} else {
		//printf("Cannot add %u: %s\n", code, result.errorText);
	}
	return result;*/
}


///////////////////////////////////


RCSPStream::RCSPStream(RCSPAggregator* aggregator, uint16_t size) :
    m_aggregator(aggregator),
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

uint8_t* RCSPStream::getStream() const
{
	return m_stream;
}

uint16_t RCSPStream::getSize() const
{
	return m_size;
}

RCSPAggregator::ResultType RCSPStream::addValue(OperationCode code)
{
    if (m_aggregator == nullptr)
    {
        static const char msg[] = "RCSPStream: Cannot add value to stream when m_aggregator == nullptr";
        error << msg;
        return RCSPAggregator::ResultType(RCSPAggregator::AGGREGATOR_IS_NULLPTR, msg);
    }
	//printf("Adding value, code %u\n", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return m_aggregator->serializePush(pos, code, m_size - m_cursor, addedSize);
		}
	);
}

RCSPAggregator::ResultType RCSPStream::addRequest(OperationCode code)
{
	//printf("Adding value, code %u\n", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return RCSPAggregator::serializePull(pos, code, m_size - m_cursor, addedSize);
		}
	);
}

RCSPAggregator::ResultType RCSPStream::addCall(OperationCode code)
{
	//printf("Adding value, code %u\n", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return RCSPAggregator::serializeCall(pos, code, m_size - m_cursor, addedSize);
		}
	);
}

RCSPAggregator::ResultType RCSPStream::serializeAnything(OperationCode code, SerializationFunc serializer)
{
	uint8_t *pos = m_stream + m_cursor;
	uint16_t addedSize = 0;
	RCSPAggregator::ResultType result = serializer(pos, code, addedSize);
	if (result.isSuccess)
	{
		m_cursor += addedSize;
	} else {
		//printf("Cannot add %u: %s\n", code, result.errorText);
	}
	return result;
}

PackageId RCSPStream::send(
    INetworkClient* client,
	DeviceAddress target,
	bool waitForAck,
	PackageSendingDoneCallback doneCallback,
	PackageTimings&& timings
)
{
	return client->send(
		target,
		m_stream,
		m_size,
		waitForAck,
		doneCallback,
		timings
	);
}

void RCSPStream::dispatch()
{
    if (m_aggregator == nullptr)
    {
        error << "RCSPStream: Cannot dispatch stream when m_aggregator == nullptr";
        return;
    }

	if (!empty())
	{
		m_aggregator->dispatchStream(m_stream, m_cursor);
	}
}


bool RCSPStream::empty()
{
	return (m_cursor == 0);
}

//////////////////////////
// RCSPMultiStream

RCSPMultiStream::RCSPMultiStream(RCSPAggregator* aggregator) :
        m_aggregator(aggregator)
{
	pushBackStream();
}

void RCSPMultiStream::pushBackStream()
{
	m_streams.push_back(std::shared_ptr<RCSPStream> (new RCSPStream(m_aggregator)));
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
        INetworkClient* client,
		DeviceAddress target,
		bool waitForAck,
		PackageTimings&& timings
	)
{
	for (auto it=m_streams.begin(); it != m_streams.end(); it++)
	{
		(*it)->send(client, target, waitForAck, nullptr, std::forward<PackageTimings>(timings));
	}
}

bool RCSPMultiStream::empty()
{
	return m_streams.front()->empty();
}

void RCSPMultiStream::dispatch()
{
	for (auto it=m_streams.begin(); it != m_streams.end(); it++)
	{
		(*it)->dispatch();
	}
}

Result RCSPMultiStream::writeToFile(FILE* file)
{
	UINT written = 0;
	for (auto it = m_streams.begin(); it != m_streams.end(); it++)
	{
        written = fwrite((*it)->getStream(), 1, (*it)->getSize(), file);
        if (ferror(file) != 0)
		{
            return Result("Bad file writing result");
		}
		if (written != (*it)->getSize())
		{
            return Result("Invalid written bytes count");
		}
	}
    return Result();
}

RCSPNetworkListener::RCSPNetworkListener(RCSPAggregator* aggregator) :
        m_aggregator(aggregator)
{

}

bool RCSPNetworkListener::hasSender()
{
	return m_hasDeviceAddress;
}

DeviceAddress RCSPNetworkListener::sender()
{
	return m_currentDeviceAddress;
}

void RCSPNetworkListener::receivePackage(DeviceAddress sender, uint8_t* payload, uint16_t payloadLength)
{
    if (m_networkClient == nullptr)
    {
        error << "Network client was not connected to package receiver! Cannot dispatch stream now";
        return;
    }
    RCSPMultiStream answerStream(m_aggregator);
    m_currentDeviceAddress = sender;
    m_hasDeviceAddress = true;
    m_aggregator->dispatchStream(payload, payloadLength, &answerStream);
    m_hasDeviceAddress = false;
    if (!answerStream.empty())
    {
        answerStream.send(m_networkClient, sender, true);
    }
}

void RCSPNetworkListener::connectClient(INetworkClient* client)
{
    m_networkClient = client;
}

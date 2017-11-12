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
#include <stdio.h>

RCSPStream::RCSPStream(RCSPAggregator* aggregator) :
	m_aggregator(aggregator)
{
}

void RCSPStream::addPush(OperationCode code, const uint8_t* customValue)
{
	m_aggregator->serializePush(code, m_buffer, customValue);
}

void RCSPStream::addPull(OperationCode code)
{
	RCSPAggregator::serializePull(code, m_buffer);
}

void RCSPStream::addCall(OperationCode code)
{
	RCSPAggregator::serializeCall(code, m_buffer);
}

PackageId RCSPStream::send(
		INetworkClient* client,
		DeviceAddress target,
		bool waitForAck,
		PackageSendingDoneCallback doneCallback,
		PackageTimings&& timings
	)
{
	PackageId lastId = 0;
	auto sender = [&lastId, client, &target, waitForAck, doneCallback, timings](const uint8_t* begin, uint16_t size)
	{
		lastId = client->send(target, begin, size, waitForAck, doneCallback, timings);
	};
	RCSPAggregator::splitBuffer(m_buffer, client->payloadSize(), sender);
	return lastId;
}

void RCSPStream::dispatch()
{
	m_aggregator->dispatchStreamNew(m_buffer.data(), m_buffer.size());
}

const Buffer& RCSPStream::buffer() const
{
	return m_buffer;
}

Buffer& RCSPStream::buffer()
{
	return m_buffer;
}

Result RCSPStream::writeToFile(FILE* file)
{
	int written = fwrite(m_buffer.data(), 1, m_buffer.size(), file);
	if (ferror(file) != 0)
	{
		return Result("Bad file writing result");
	}
	if (written != m_buffer.size())
	{
		return Result("Invalid written bytes count");
	}
	return Result();
}

//////////////////////////////
// RCSPNetworkListener

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

void RCSPNetworkListener::receivePackage(DeviceAddress sender, const uint8_t* payload, uint16_t payloadLength)
{
    if (m_networkClient == nullptr)
    {
        error << "Network client was not connected to package receiver! Cannot dispatch stream now";
        return;
    }
    RCSPStream answerStream(m_aggregator);
    m_currentDeviceAddress = sender;
    m_hasDeviceAddress = true;
    m_aggregator->dispatchStreamNew(payload, payloadLength, &(answerStream.buffer()));
    m_hasDeviceAddress = false;
    if (!answerStream.buffer().empty())
    {
        answerStream.send(m_networkClient, sender, true);
    }
}

void RCSPNetworkListener::connectClient(INetworkClient* client)
{
    m_networkClient = client;
}


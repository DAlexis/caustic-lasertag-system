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

#include "rcsp/RCSP-stream.hpp"
#include "core/string-utils.hpp"
#include "fatfs.h"
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
	//printf("Adding value, code %u\n", code);
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
	//printf("Adding value, code %u\n", code);
	return serializeAnything(
		code,
		[this] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
		{
			return RCSPAggregator::instance().serializeObjectRequest(pos, code, m_size - m_cursor, addedSize);
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
			return RCSPAggregator::instance().serializeCallRequest(pos, code, m_size - m_cursor, addedSize);
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
	DeviceAddress target,
	bool waitForAck,
	PackageSendingDoneCallback doneCallback,
	PackageTimings&& timings
)
{
	return NetworkLayer::instance().send(
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
	if (!empty())
	{
		RCSPAggregator::instance().dispatchStream(m_stream, m_cursor);
	}
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
		bool waitForAck,
		PackageTimings&& timings
	)
{
	for (auto it=m_streams.begin(); it != m_streams.end(); it++)
	{
		(*it)->send(target, waitForAck, nullptr, std::forward<PackageTimings>(timings));
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

DetailedResult<FRESULT> RCSPMultiStream::writeToFile(FIL* file)
{
	FRESULT res = FR_OK;
	UINT written = 0;
	for (auto it = m_streams.begin(); it != m_streams.end(); it++)
	{
		res = f_write (file, (*it)->getStream(), (*it)->getSize(), &written);
		if (res != FR_OK)
		{
			return DetailedResult<FRESULT>(res, "Bad file writing result");
		}
		if (written != (*it)->getSize())
		{
			return DetailedResult<FRESULT>(res, "Invalid written bytes count");
		}
	}
}


ReceivePackageCallback RCSPMultiStream::getPackageReceiver()
{
	return [](DeviceAddress sender, uint8_t* payload, uint16_t payloadLength)
	{
		RCSPMultiStream answerStream;
		RCSPAggregator::instance().dispatchStream(payload, payloadLength, &answerStream);
		if (!answerStream.empty())
		{
			answerStream.send(sender, true);
		}
	};
}

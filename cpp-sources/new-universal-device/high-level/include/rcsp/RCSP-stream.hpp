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

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_

#include "rcsp/RCSP-aggregator.hpp"
#include "network/network-layer.hpp"
#include "fatfs.h"
#include "core/result-code.hpp"
#include <list>
#include <memory>

class RCSPStream
{
public:
	constexpr static uint16_t defaultLength = Package::payloadLength;
	RCSPStream(uint16_t size = defaultLength);
	~RCSPStream();

	static PackageId remoteCall(
			DeviceAddress target,
			OperationCode code,
			bool waitForAck = true,
			PackageSendingDoneCallback callback = nullptr,
			PackageTimings&& timings = PackageTimings()
			)
	{
		RCSPStream stream;
		stream.addCall(code);
		return stream.send(target, waitForAck, callback, std::forward<PackageTimings>(timings));
	}

	template <typename Type>
	static PackageId remoteCall(
			DeviceAddress target,
			OperationCode code,
			Type& argument,
			bool waitForAck = true,
			PackageSendingDoneCallback callback = nullptr,
			PackageTimings&& timings = PackageTimings()
			)
	{
		RCSPStream stream;
		stream.addCall(code, argument);
		return stream.send(target, waitForAck, callback, std::forward<PackageTimings>(timings));
	}

	static PackageId remotePullValue(
			DeviceAddress target,
			OperationCode code,
			bool waitForAck = true,
			PackageSendingDoneCallback callback = nullptr,
			PackageTimings&& timings = PackageTimings()
			)
	{
		RCSPStream stream;
		stream.addValue(code);
		return stream.send(target, waitForAck, callback, std::forward<PackageTimings>(timings));
	}

	uint8_t* getStream() const;
	uint16_t getSize() const;
	RCSPAggregator::ResultType addValue(OperationCode code);
	RCSPAggregator::ResultType addRequest(OperationCode code);
	RCSPAggregator::ResultType addCall(OperationCode code);

	template<typename Type>
	RCSPAggregator::ResultType addCall(OperationCode code, const Type& arg)
	{
		//printf("Adding value, code %u", code);
		return serializeAnything(
			code,
			[this, &arg] (uint8_t *pos, OperationCode code, uint16_t &addedSize) -> RCSPAggregator::ResultType
			{
				return RCSPAggregator::instance().serializeCallRequest(pos, code, m_size - m_cursor, addedSize, arg);
			}
		);
	}

	/**
	 * Send current stream with specified timeout, resend period and resend period delta
	 * @TODO Move sending function from stream. Stream only for multiply serializetion
	 * @param target Address of receiver
	 * @param waitForAck Need resend while no ACK received
	 * @param doneCallback Callback after successful/not successful delivery
	 * @param timings Package timings
	 * @return Package id
	 */
	PackageId send(
		DeviceAddress target,
		bool waitForAck = false,
		PackageSendingDoneCallback doneCallback = nullptr,
		PackageTimings&& timings = PackageTimings()
	);

	void dispatch();

	bool empty();

private:

	using SerializationFunc = std::function<RCSPAggregator::ResultType (uint8_t * /*pos*/, OperationCode /*code*/, uint16_t & /*addedSize*/)>;
	RCSPAggregator::ResultType serializeAnything(OperationCode code, SerializationFunc serializer);
	uint8_t* m_stream = nullptr;
	uint16_t m_cursor = 0;
	uint16_t m_freeSpace = 0;
	uint16_t m_size = 0;
};

class RCSPMultiStream
{
public:
	RCSPMultiStream();
	RCSPAggregator::ResultType addValue(OperationCode code);
	RCSPAggregator::ResultType addRequest(OperationCode code);
	RCSPAggregator::ResultType addCall(OperationCode code);

	template<typename Type>
	RCSPAggregator::ResultType addCall(OperationCode code, const Type& arg)
	{
		RCSPAggregator::ResultType result = m_streams.back()->addCall(code, arg);
		if (!result.isSuccess && result.details == RCSPAggregator::NOT_ENOUGH_SPACE)
		{
			pushBackStream();
			result = m_streams.back()->addCall(code, arg);
		}
		return result;
	}

	void send(
		DeviceAddress target,
		bool waitForAck = false,
		PackageTimings&& timings = PackageTimings()
	);

	bool empty();

	void dispatch();

	DetailedResult<FRESULT> writeToFile(FIL* file);

private:
	void pushBackStream();
	std::list<std::shared_ptr<RCSPStream>> m_streams;
};

class RCSPNetworkListener
{
public:
	SINGLETON_IN_CLASS(RCSPNetworkListener);
	ReceivePackageCallback getPackageReceiver();

	bool hasSender();
	DeviceAddress sender();

private:
	RCSPNetworkListener();
	void packagesReceiver(DeviceAddress sender, uint8_t* payload, uint16_t payloadLength);
	bool m_hasDeviceAddress = false;
	DeviceAddress m_currentDeviceAddress;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_ */

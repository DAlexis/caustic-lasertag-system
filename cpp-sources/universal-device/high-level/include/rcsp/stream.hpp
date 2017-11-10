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

#include "rcsp/aggregator.hpp"
#include "network/network-client.hpp"
#include "core/result-code.hpp"
#include <list>
#include <memory>

class RCSPStreamNew
{
public:
	RCSPStreamNew(RCSPAggregator* aggregator);
	void addPush(OperationCode code, const uint8_t* customValue = nullptr);
	void addPull(OperationCode code);
	void addCall(OperationCode code);
	template <typename T>
	void addCall(OperationCode code, const T& argument)
	{
		RCSPAggregator::serializeCall(code, m_buffer, argument);
	}

	PackageId send(
		INetworkClient* client,
		DeviceAddress target,
		bool waitForAck = false,
		PackageSendingDoneCallback doneCallback = nullptr,
		PackageTimings&& timings = PackageTimings()
	);

	static PackageId remoteCall(
			INetworkClient* client,
			DeviceAddress target,
			OperationCode code,
			bool waitForAck = true,
			PackageSendingDoneCallback callback = nullptr,
			PackageTimings&& timings = PackageTimings()
			)
	{
		RCSPStreamNew stream(nullptr);
		stream.addCall(code);
		return stream.send(client, target, waitForAck, callback, std::forward<PackageTimings>(timings));
	}

	template <typename Type>
	static PackageId remoteCall(
			INetworkClient* client,
			DeviceAddress target,
			OperationCode code,
			Type& argument,
			bool waitForAck = true,
			PackageSendingDoneCallback callback = nullptr,
			PackageTimings&& timings = PackageTimings()
			)
	{
		RCSPStreamNew stream(nullptr);
		stream.addCall(code, argument);
		return stream.send(client, target, waitForAck, callback, std::forward<PackageTimings>(timings));
	}

	static PackageId remotePush(
			RCSPAggregator* aggregator,
			INetworkClient* client,
			DeviceAddress target,
			OperationCode code,
			bool waitForAck = true,
			PackageSendingDoneCallback callback = nullptr,
			PackageTimings&& timings = PackageTimings()
			)
	{
		RCSPStreamNew stream(aggregator);
		stream.addPush(code);
		return stream.send(client, target, waitForAck, callback, std::forward<PackageTimings>(timings));
	}

	void dispatch();

	const Buffer& buffer() const;
	Buffer& buffer();
    Result writeToFile(FILE* file);

private:
    Buffer m_buffer;
	RCSPAggregator* m_aggregator;
};

class RCSPNetworkListener : public IPackageReceiver
{
public:
    RCSPNetworkListener(RCSPAggregator* aggregator = nullptr);

	bool hasSender();
	DeviceAddress sender();

	void receivePackage(DeviceAddress sender, const uint8_t* payload, uint16_t payloadLength) override;
    void connectClient(INetworkClient* client) override;

private:
	bool m_hasDeviceAddress = false;
	DeviceAddress m_currentDeviceAddress;
	INetworkClient* m_networkClient = nullptr;
	RCSPAggregator* m_aggregator = nullptr;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RCSP_STREAM_HPP_ */

/*
 * network-client-mock.cpp
 *
 *  Created on: 12 нояб. 2017 г.
 *      Author: dalexies
 */

#include "network-client-mock.hpp"

void NetworkClientMock::connectNetworkLayer(INetworkLayer* nl)
{
}

bool NetworkClientMock::isForMe(const DeviceAddress& addr)
{
}

void NetworkClientMock::receive(DeviceAddress sender, const uint8_t* payload, uint16_t payloadLength)
{
}

const DeviceAddress* NetworkClientMock::mainBackAddress()
{
}

void NetworkClientMock::connectPackageReceiver(IPackageReceiver* receiver)
{
}

PackageId NetworkClientMock::send(
	DeviceAddress target,
	const uint8_t* data,
	uint16_t size,
	bool waitForAck,
	PackageSendingDoneCallback doneCallback,
	PackageTimings timings
)
{
}


uint16_t NetworkClientMock::payloadSize()
{
}

/*
 * network-layer-moc.cpp
 *
 *  Created on: 17 окт. 2017 г.
 *      Author: dalexies
 */

#include "network-layer-mock.hpp"
#include <cstring>

void NetworkLayerMock::start(IRadioPhysicalDevice* rfPhysicalDevice)
{
}

void NetworkLayerMock::connectClient(INetworkClient* client)
{
}

void NetworkLayerMock::setRadioReinitCallback(RadioReinitCallback callback)
{
}



PackageId NetworkLayerMock::send(
	DeviceAddress target,
	DeviceAddress sender,
	const uint8_t* data,
	uint16_t size,
	bool waitForAck,
	PackageSendingDoneCallback doneCallback,
	PackageTimings timings,
	INetworkClient* doNotReceiveBy
)
{
	m_sendedBuffer.resize(size);
	memcpy(m_sendedBuffer.data(), data, size);
	return 1;
}



uint16_t NetworkLayerMock::payloadSize()
{
	return 23;
}



bool NetworkLayerMock::stopSending(PackageId packageId)
{
	return true;
}


void NetworkLayerMock::dropAllForAddress(const DeviceAddress& address)
{
}


bool NetworkLayerMock::updateTimeout(PackageId packageId)
{
	return true;
}



void NetworkLayerMock::registerBroadcast(const DeviceAddress& address)
{
}


void NetworkLayerMock::registerBroadcastTester(Broadcast::IBroadcastTester* tester)
{
}


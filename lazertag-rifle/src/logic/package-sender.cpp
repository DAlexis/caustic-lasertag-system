/*
 * package-sender.cpp
 *
 *  Created on: 28 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/package-sender.hpp"

#include <stdio.h>
#include <string.h>

void PackageSender::init()
{
	printf("Package sender initialization...\n");
	nrf.setTXDoneCallback(std::bind(&PackageSender::TXDoneCallback, this));
	nrf.setDataReceiveCallback(std::bind(&PackageSender::RXCallback, this, std::placeholders::_1, std::placeholders::_2));
	nrf.init(
		IOPins->getIOPin(1, 7),
		IOPins->getIOPin(1, 12),
		IOPins->getIOPin(1, 8),
		nullptr,
		SPIs->getSPI(1)
	);
	nrf.printStatus();
}

uint16_t PackageSender::generatePackageId()
{
	uint16_t id = (systemClock->getTime()) & 0x3FFF;
	if (id == 0)
		id = 1;
	return id;
}


uint16_t PackageSender::send(DeviceAddress target, uint8_t* data, uint16_t size, bool waitForAck, PackageSendingDoneCallback doneCallback)
{
	if (size>Package::payloadLength)
		return 0;
	PackageIdAndTTL idAndTTL;
	idAndTTL.packageId = generatePackageId();

	if (waitForAck)
	{
		Time time = systemClock->getTime();
		idAndTTL.TTL = 0;
		m_packages[idAndTTL.packageId] = WaitingPackage();
		m_packages[idAndTTL.packageId].wasCreated = time;
		m_packages[idAndTTL.packageId].nextTransmission = time;
		m_packages[idAndTTL.packageId].callback = doneCallback;
		m_packages[idAndTTL.packageId].package.sender = self;
		m_packages[idAndTTL.packageId].package.target = target;
		m_packages[idAndTTL.packageId].package.idAndTTL = idAndTTL;
	} else {
		idAndTTL.TTL = 0;
		m_packagesNoAck.push_back(Package());
		m_packagesNoAck.back().sender = self;
		m_packagesNoAck.back().target = target;
		m_packagesNoAck.back().idAndTTL = idAndTTL;
		memcpy(m_packagesNoAck.back().payload, data, size);
		if (size<Package::payloadLength)
			memset(m_packagesNoAck.back().payload+size, 0, size-Package::payloadLength);
		return 0;
	}
}

void PackageSender::TXDoneCallback()
{
	isSendingNow = false;
	if (currentlySendingPackageId != 0)
	{
		PackageSendingDoneCallback callback = m_packages[currentlySendingPackageId].callback;
		m_packages.erase(currentlySendingPackageId);
		callback(currentlySendingPackageId, true);
	}
}

void PackageSender::RXCallback(uint8_t channel, uint8_t* data)
{
	Package received;
	memcpy(&received, data, sizeof(Package));
	if (received.target != self)
		return;
	// Dispatching if this is acknoledgement
	uint16_t firstOpCode;
	memcpy(&firstOpCode, received.payload, sizeof(uint16_t));
	//if (firstOpCode == ConfigCodes::acknoledgement)
}


void PackageSender::interrogate()
{
	if (isSendingNow)
		return;
	// First, sending packages without response
	if (!m_packagesNoAck.empty())
	{
		nrf.sendData(Package::packageLength, (uint8_t*) &(m_packagesNoAck.front()));
		m_packagesNoAck.pop_front();
		isSendingNow = true;
		// Tell that now we are sending no-response package
		currentlySendingPackageId = 0;
		return;
	}
	Time time = systemClock->getTime();
	for (auto it=m_packages.begin(); it!=m_packages.end(); it++)
	{
		// If it is time to (re)send package
		if (it->second.nextTransmission < time)
		{
			currentlySendingPackageId = it->first;
			isSendingNow = true;
			nrf.sendData(Package::packageLength, (uint8_t*) &(it->second.package));
			return;
		}
	}
}

/*
 * package-sender.cpp
 *
 *  Created on: 28 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/package-sender.hpp"

#include "core/scheduler.hpp"

#include "dev/random.hpp"

#include <stdio.h>
#include <string.h>

PackageSender* PackageSender::m_packageSender = nullptr;
STATIC_DEINITIALIZER_IN_CPP_FILE(PackageSender, m_packageSender)

void PackageSender::init()
{
	printf("Package sender initialization...\n");
	nrf.setTXDoneCallback(std::bind(&PackageSender::TXDoneCallback, this));
	nrf.setDataReceiveCallback(std::bind(&PackageSender::RXCallback, this, std::placeholders::_1, std::placeholders::_2));
	IExternalInterruptManager *exti = EXTIS->getEXTI(8);
	exti->init(1);
	nrf.init(
		IOPins->getIOPin(1, 7),
		IOPins->getIOPin(1, 12),
		IOPins->getIOPin(1, 8),
		exti,
		SPIs->getSPI(1)
	);
	nrf.printStatus();
	Scheduler::instance().addTask(std::bind(&PackageSender::interrogate, this), false, 10000, 10000);
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
	if (size > Package::payloadLength)
	{
		printf("Error: too long payload!\n");
		return 0;
	}
	PackageIdAndTTL idAndTTL;
	idAndTTL.packageId = generatePackageId();

	if (waitForAck)
	{
		Time time = systemClock->getTime();
		m_packages[idAndTTL.packageId] = WaitingPackage();
		WaitingPackage& waitingPackage = m_packages[idAndTTL.packageId];
		waitingPackage.wasCreated = time;
		waitingPackage.nextTransmission = time;
		waitingPackage.callback = doneCallback;
		waitingPackage.package.sender = self;
		waitingPackage.package.target = target;
		waitingPackage.package.idAndTTL = idAndTTL;
		memcpy(waitingPackage.package.payload, data, size);
		printf("Ack-using package queued\n");
		return idAndTTL.packageId;
	} else {
		m_packagesNoAck.push_back(Package());
		m_packagesNoAck.back().sender = self;
		m_packagesNoAck.back().target = target;
		m_packagesNoAck.back().idAndTTL = idAndTTL;
		memcpy(m_packagesNoAck.back().payload, data, size);
		if (size<Package::payloadLength)
			memset(m_packagesNoAck.back().payload+size, 0, size-Package::payloadLength);
		printf("No-ack package queued\n");
		return 0;
	}
}

void PackageSender::TXDoneCallback()
{
	isSendingNow = false;
}

void PackageSender::RXCallback(uint8_t channel, uint8_t* data)
{
	Package received;
	memcpy(&received, data, sizeof(Package));

	// Skipping packages for other devices
	if (received.target != self)
		return;

	// Dispatching if this is acknoledgement
	AckPayload *ackDispatcher = reinterpret_cast<AckPayload *>(received.payload);
	if (ackDispatcher->isAck())
	{
		printf("Ack package received for id=%u\n", ackDispatcher->packageId);
		auto it = m_packages.find(ackDispatcher->packageId);
		if (it == m_packages.end())
		{
			// No packages waiting this ack
			return;
		}
		PackageSendingDoneCallback callback = it->second.callback;
		m_packages.erase(it);
		printf("Package removed from queue\n");
		if (callback)
			callback(ackDispatcher->packageId, true);
		return;
	}

	printf("Received package with id=%u\n", received.idAndTTL.packageId);
	// Generating acknledgement

	// Forming payload for ack package
	AckPayload ackPayload;
	ackPayload.packageId = received.idAndTTL.packageId;
	// Forming ack package
	Package ack;
	ack.target = received.sender;
	ack.sender = self;
	ack.idAndTTL.packageId = generatePackageId();
	memcpy(&ack.payload, &ackPayload, sizeof(ackPayload));
	memset(ack.payload+sizeof(ackPayload), 0, ack.payloadLength - sizeof(ackPayload));
	// Adding ack package to list for sending
	m_packagesNoAck.push_back(ack);

	// Putting received package to list
	m_incoming.push_back(received);
}

void PackageSender::interrogate()
{
	nrf.interrogate();
	if (!isSendingNow)
	{
		sendNext();
	}
	while (!m_incoming.empty())
	{
		ConfigsAggregator::instance().dispatchStream(m_incoming.front().payload, m_incoming.front().payloadLength);
		m_incoming.pop_front();
	}
}

void PackageSender::sendNext()
{
	// First, sending packages without response
	if (!m_packagesNoAck.empty())
	{
		printf("Sending package with NO ack needed\n");
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
		// If timeout
		if (time - it->second.wasCreated > timeout)
		{
			PackageSendingDoneCallback callback = it->second.callback;
			uint16_t timeoutedPackageId = it->second.package.idAndTTL.packageId;
			printf("Package %u timeouted\n", timeoutedPackageId);
			m_packages.erase(it);
			if (callback)
				callback(timeoutedPackageId, false);

			/// @todo Improve code somehow and remove this return
			// Return because iterators are bad now
			return;
		}
		// If it is time to (re)send package
		if (it->second.nextTransmission < time)
		{
			printf("Sending package with ack needed, id=%u\n", it->second.package.idAndTTL.packageId);
			currentlySendingPackageId = it->first;
			isSendingNow = true;
			it->second.nextTransmission = time+resendTime+Random::random(resendTime / 2);
			nrf.sendData(Package::packageLength, (uint8_t*) &(it->second.package));
			return;
		}
	}
}

PackageSender& PackageSender::instance()
{
	if (!m_packageSender)
		m_packageSender = new PackageSender;
	return *m_packageSender;
}

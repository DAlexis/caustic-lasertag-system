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

#include "network/network-layer.hpp"
#include "network/broadcast.hpp"
#include "network/network-client.hpp"

#include "core/os-wrappers.hpp"
#include "core/logging.hpp"
#include "core/string-utils.hpp"

#include "dev/random.hpp"

#include <stdio.h>
#include <string.h>


///////////////////////
// NetworkLayer
NetworkLayer::NetworkLayer()
{
	m_modemTask.setStackSize(1024);
	m_modemTask.setName("NetLay");
	m_modemTask.setTask(std::bind(&NetworkLayer::interrogate, this));
}

NetworkLayer::~NetworkLayer()
{
	for (auto it = m_broadcastTesters.begin(); it != m_broadcastTesters.end(); it++)
	{
		if (*it)
			delete *it;
	}
}

void NetworkLayer::setRadioReinitCallback(RadioReinitCallback callback)
{
	m_radioReinitCallback = callback;
}

void NetworkLayer::start(IRadioPhysicalDevice* rfPhysicalDevice)
{
	m_rfPhysicalDevice = rfPhysicalDevice;
	m_rfPhysicalDevice->setTXDoneCallback(std::bind(&NetworkLayer::TXDoneCallback, this));
	m_rfPhysicalDevice->setDataReceiveCallback(std::bind(&NetworkLayer::RXCallback, this, std::placeholders::_1, std::placeholders::_2));
	info << "Starting m_modemTask";
	m_modemTask.run(0, 0, 0);
}

void NetworkLayer::connectClient(INetworkClientReceiver* client)
{
    client->connectNetworkLayer(this);
    m_clients.push_back(client);
}

void NetworkLayer::registerBroadcast(const DeviceAddress& address)
{
	m_broadcasts.insert(address);
}

void NetworkLayer::registerBroadcastTester(Broadcast::IBroadcastTester* tester)
{
	m_broadcastTesters.push_back(tester);
}

void NetworkLayer::enableDebug(bool debug)
{
	m_debug = debug;
}

INetworkClientReceiver* NetworkLayer::findClient(const DeviceAddress& target)
{
    for (auto &it : m_clients)
    {
        if (it->isForMe(target))
        {
            return it;
        }
    }
    return nullptr;
}

uint16_t NetworkLayer::generatePackageId()
{
	uint16_t id = (systemClock->getTime()) & 0xFFFF;
	if (id == 0 || id == 1)
		id = 2;
	return id;
}

PackageId NetworkLayer::send(
        DeviceAddress target,
        DeviceAddress sender,
        const uint8_t* data,
        uint16_t size,
        bool waitForAck,
        PackageSendingDoneCallback doneCallback,
        PackageTimings timings,
        INetworkClientReceiver* doNotReceiveBy
    )
{
    m_stager.stage("send()");
    if (size == 0)
    {
    	warning << "Sending package with zero size is impossible";
    	return 0;
    }
    if (size > Package::payloadLength)
    {
        error << "Error: too long payload!";
        return 0;
    }
    // Looking for receiver on local host
    INetworkClientReceiver* localReceiverClient = findClient(target);
    if (localReceiverClient && localReceiverClient != doNotReceiveBy)
    {
    	if (radio.isEnabled())
    		trace << "Forwarding to local receiver client, target = " << ADDRESS_TO_STREAM(target);
        localReceiverClient->receive(sender, data, size);

        if (!Broadcast::isBroadcast(target))
        {
            // Package is not broadcast and target is found so it was successfuly delivered
            return 1;
        }
    }

    if (!hasFreeSpaceInQueues())
	{
		error << "Packages queue is full! Skipping package!";
		return 0;
	}

    PackageDetails details;
    details.packageId = generatePackageId();

    if (waitForAck)
    {
        Time time = systemClock->getTime();
        details.needAck = 1;

        ScopedLock<Mutex> lock(m_packagesQueueMutex);
        m_stager.stage("send(): +ack lock");

        m_waitingPackagesWithAck[details.packageId] = WaitingPackage();
        WaitingPackage& waitingPackage = m_waitingPackagesWithAck[details.packageId];
        waitingPackage.wasCreated = time;
        waitingPackage.nextTransmission = time;
        waitingPackage.isBroadcast = Broadcast::isBroadcast(target);

        waitingPackage.timings = timings;

        waitingPackage.callback = doneCallback;
        waitingPackage.package.sender = sender;
        waitingPackage.package.target = target;
        waitingPackage.package.details = details;
        memcpy(waitingPackage.package.payload, data, size);
        /// @todo Check package zerification!
        if (size<Package::payloadLength)
        {
            memset(&(waitingPackage.package.payload[size]), 0, Package::payloadLength-size);
        }
        //trace << "Ack-using package queued";
        m_stager.stage("send(): +ack queued");
        return details.packageId;
    } else {
        ScopedLock<Mutex> lock(m_packagesQueueMutex);
        m_stager.stage("send(): -ack lock");
        m_waitingPackagesNoAck.push_back(Package());
        m_waitingPackagesNoAck.back().sender = sender;
        m_waitingPackagesNoAck.back().target = target;
        m_waitingPackagesNoAck.back().details = details;
        memcpy(m_waitingPackagesNoAck.back().payload, data, size);
        if (size<Package::payloadLength)
        {
            memset(m_waitingPackagesNoAck.back().payload+size, 0, Package::payloadLength-size); //< fixed third argument
        }
        m_stager.stage("send(): -ack queued");
        //trace << "No-ack package queued";
        return 0;
    }
}

void NetworkLayer::TXDoneCallback()
{
	isSendingNow = false;
	/// @TODO be shure that this line improve stability or remove it
	temproraryProhibitTransmission(1000);
}

void NetworkLayer::RXCallback(uint8_t channel, uint8_t* data)
{
	UNUSED_ARG(channel);
	m_stager.stage("RXCallback()");
	Package received;
	memcpy(&received, data, sizeof(Package));

	temproraryProhibitTransmission(8000);

	// Skipping packages for other devices

	// New approach
	INetworkClientReceiver* receiverClient = findClient(received.target);
	if (receiverClient == nullptr)
	    return; // Not for me.

	DeviceAddress backAddr = *receiverClient->mainBackAddress();

	if (trace.isEnabled() && radio.isEnabled())
	{
		trace << "<== incoming package: " << hexStr((const uint8_t*) &received, sizeof(received));
	}
	// Dispatching if this is acknoledgement
	AckPayload *ackDispatcher = reinterpret_cast<AckPayload *>(received.payload);
	if (ackDispatcher->isAck())
	{
		radio << "<== Ack for " << ackDispatcher->packageId << " from " << ADDRESS_TO_STREAM(received.sender);
		ScopedLock<Mutex> lock(m_packagesQueueMutex);
		auto it = m_waitingPackagesWithAck.find(ackDispatcher->packageId);
		if (it == m_waitingPackagesWithAck.end())
		{
			radio << "?=? No package with id " << ackDispatcher->packageId << " waiting ack";
			// No packages waiting this ack
			return;
		}
		radio << "|=| Ack for " << ackDispatcher->packageId << " accepted";
		if (!it->second.isBroadcast)
		{
			PackageSendingDoneCallback callback = it->second.callback;
			m_waitingPackagesWithAck.erase(it);
			if (callback)
				callback(ackDispatcher->packageId, true);
		} else {
			radio << "|=| but package " << ackDispatcher->packageId << " is broadcast, so don\'t delete from queue";
		}
		return;
	}

	//printf("Received package with id=%u\n", received.idAndTTL.packageId);
	// Generating acknledgement
	if (received.details.needAck == 1)
	{
		// Forming payload for ack package
		AckPayload ackPayload;
		ackPayload.packageId = received.details.packageId;
		// Forming ack package
		Package ack;
		ack.target = received.sender;
		ack.sender = backAddr; // We already get it from one of registered INetworkClient s
		ack.details.packageId = generatePackageId();
		memcpy(&ack.payload, &ackPayload, sizeof(ackPayload));
		memset(ack.payload+sizeof(ackPayload), 0, ack.payloadLength - sizeof(ackPayload));
		ScopedLock<Mutex> lock(m_packagesQueueMutex);
		// Adding ack package to list for sending
		m_waitingPackagesNoAck.push_back(ack);
	}

	if (checkIfIdStoredAndStore(received.details.packageId))
	{
		radio << "?=? Package with id " << received.details.packageId << " repetition detected";
	} else {
		if (hasFreeSpaceInQueues())
		{
			// Putting received package to list
			m_incoming.push_back(received);
		} else {
			error << "No free space in queues to receive incoming package!";
		}
	}
}

bool NetworkLayer::checkIfIdStoredAndStore(uint16_t id)
{
	uint16_t currentSize = 0;
	for (auto it = m_lastReceivedIds.begin(); it != m_lastReceivedIds.end(); it++, currentSize++)
	{
		if (*it == id)
			return true;
	}
	m_lastReceivedIds.push_back(id);
	if (currentSize == lastReceivedIdsBufferSize) {
		m_lastReceivedIds.pop_front();
	}
	return false;
}

void NetworkLayer::interrogate()
{
	sendNext();
	m_rfPhysicalDevice->interrogate();
	receiveIncoming();
/*
	if ((systemClock->getTime() - m_lastNRFReinitializationTime) > NRFReinitPeriod)
	{
		if (m_debug)
			m_rfPhysicalDevice->printStatus();

		if (m_regularNRFReinit)
		{
			m_stager.stage("interrogate(): reinit module");
			debug << "NRF module reinitializing to prevent broken states";
			reinitNRF();
		}
		m_lastNRFReinitializationTime = systemClock->getTime();
	}
*/

	/// @todo Remove comment below after some time
	/*
	static int i=0;
	i++;
	if (i==1500) {
		i = 0;
		radio << "Net Alive!";
		if (isSendingNow)
			radio << "SENDING";

		m_rfPhysicalDevice->printStatus();

		//reinitNRF();
	}*/
}

void NetworkLayer::sendNext()
{
	m_stager.stage("sendNext()");
	if (!isTranslationAllowed())
	{
		m_stager.stage("sendNext(): Transmission disallowed");
		return;
	}
	ScopedLock<Mutex> lock(m_packagesQueueMutex);
	// First, sending packages without response
	if (!m_waitingPackagesNoAck.empty())
	{
		m_stager.stage("sendNext(): sending no ack");
		// If it is ack (only for output, may be removed by #ifdef DEBUG later)
		AckPayload *ackDispatcher = reinterpret_cast<AckPayload *>(m_waitingPackagesNoAck.front().payload);
		if (ackDispatcher->isAck())
		{
			radio << "==> Ack to " << ackDispatcher->packageId << " for " << ADDRESS_TO_STREAM(m_waitingPackagesNoAck.front().target) << " ";
		} else {
			radio << "==> No-ack package " << m_waitingPackagesNoAck.front().details.packageId << " for " << ADDRESS_TO_STREAM(m_waitingPackagesNoAck.front().target) << " ";
		}

		printAndSend(m_waitingPackagesNoAck.front());

		m_waitingPackagesNoAck.pop_front();
		isSendingNow = true;
		// Tell that now we are sending no-response package
		currentlySendingPackageId = 0;
		return;
	}

	Time time = systemClock->getTime();
	for (auto it=m_waitingPackagesWithAck.begin(); it!=m_waitingPackagesWithAck.end(); )
		// ++it in the end of cycle body for case when we removing outdated packages
	{
		// If timeout
		if (!it->second.timings.infiniteResend && time - it->second.wasCreated > it->second.timings.timeout)
		{
			PackageSendingDoneCallback callback = it->second.callback;
			uint16_t timeoutedPackageId = it->second.package.details.packageId;
			radio << "==| Package " << timeoutedPackageId << " timeouted";
			m_waitingPackagesWithAck.erase(it++); // We can increment iterator
			if (callback)
				callback(timeoutedPackageId, false);
			continue; // we alredy did ++it so go to next iteration
		}
		// If it is time to (re)send package
		if (it->second.nextTransmission < time)
		{
			m_stager.stage("sendNext(): sending with ack");
			radio << "==> Need-ack package " << it->second.package.details.packageId << " for " << ADDRESS_TO_STREAM(it->second.package.target);
			//radio << "Transmitting " << it->second.package.details.packageId;
			currentlySendingPackageId = it->first;
			isSendingNow = true;
			it->second.nextTransmission = time + it->second.timings.resendTime + Random::random(it->second.timings.resendTimeDelta);

			printAndSend(it->second.package);

			return;
		}
		++it; // Now can increment
	}
}

void NetworkLayer::receiveIncoming()
{
	while (!m_incoming.empty())
	{
		/// @todo [Refactoring] Remove stream from there

		radio << "<== Incoming id " << m_incoming.front().details.packageId
			<< " from " << ADDRESS_TO_STREAM(m_incoming.front().sender)
			<< " need ack: " << m_incoming.front().details.needAck;

		// New approach
		for (auto &it : m_clients)
		{
		    if (it->isForMe(m_incoming.front().target))
		    {
		        it->receive(m_incoming.front().sender, m_incoming.front().payload, m_incoming.front().payloadLength);
		        break;
		    }
		}

		m_incoming.pop_front();
	}
}

bool NetworkLayer::isTranslationAllowed()
{
	return (
			!isSendingNow &&
			(systemClock->getTime() - m_transmissionProhibitedTime) > m_transmissionProhibitionPeriod
			);
}

void NetworkLayer::temproraryProhibitTransmission(uint32_t delay)
{
	m_transmissionProhibitedTime = systemClock->getTime();
	/**
	 * This variable might be optimized. This value means time delay between last receiving and first transmission.
	 *
	 * 4000 is too small even for two devices in network: at least some acks does not reach
	 * 8000 is enough, but may be too large
	 *
	 */
	m_transmissionProhibitionPeriod = delay;
}

bool NetworkLayer::isBroadcast(const DeviceAddress& addr)
{
	if (m_broadcasts.find(addr) != m_broadcasts.end())
		return true;
	for (auto it = m_broadcastTesters.begin(); it != m_broadcastTesters.end(); it++)
		if (*it && (*it)->isAcceptableBroadcast(addr))
			return true;
	return false;
}

void NetworkLayer::printAndSend(Package& package)
{
	m_stager.stage("printAndSend()");
	if (trace.isEnabled() && radio.isEnabled())
	{
		trace << "payload to physical: " << hexStr((const uint8_t*) &package, Package::packageLength);
	}
	m_rfPhysicalDevice->sendData(Package::packageLength, (uint8_t*) &package);
}

uint16_t NetworkLayer::payloadSize()
{
	return m_rfPhysicalDevice->getPayloadSize() - sizeof(PackageHeader);
}

bool NetworkLayer::stopSending(PackageId packageId)
{
	ScopedLock<Mutex> lock(m_packagesQueueMutex);
	auto it = m_waitingPackagesWithAck.find(packageId);
	if (it != m_waitingPackagesWithAck.end())
	{
		m_waitingPackagesWithAck.erase(it);
		return true;
	}
	return false;
}

void NetworkLayer::dropAllForAddress(const DeviceAddress& address)
{
	ScopedLock<Mutex> lock(m_packagesQueueMutex);
	for(auto it = m_waitingPackagesWithAck.begin(); it != m_waitingPackagesWithAck.end(); )
	{
		if (it->second.package.target == address) {
			m_waitingPackagesWithAck.erase(it++);
		} else {
			++it;
	}
}
}

bool NetworkLayer::updateTimeout(PackageId packageId)
{
	ScopedLock<Mutex> lock(m_packagesQueueMutex);
	auto it = m_waitingPackagesWithAck.find(packageId);
	if (it != m_waitingPackagesWithAck.end())
	{
		it->second.wasCreated = systemClock->getTime();
		return true;
	}
	return false;
}


bool NetworkLayer::hasFreeSpaceInQueues()
{
	uint32_t na = m_waitingPackagesNoAck.size();
	trace << "No ack packages queue size = " << na;
	uint32_t wa = m_waitingPackagesWithAck.size();
	trace << "Need ack packages queue size = " << wa;
	uint32_t ni = m_incoming.size();
	trace << "Incoming packages queue size = " << ni;
	return na + wa + ni < maxPackagesQueueSize;
}
/// @todo Remove this code after some time

void NetworkLayer::reinitNRF()
{
	TXDoneCallback();
	if (m_radioReinitCallback)
		m_radioReinitCallback(m_rfPhysicalDevice);
	else
		warning << "Radio re-initialization callback was not set but it is time to reinit";
}

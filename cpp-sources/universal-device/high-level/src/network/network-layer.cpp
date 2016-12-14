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

#include "core/os-wrappers.hpp"
#include "core/logging.hpp"
#include "core/string-utils.hpp"

#include "dev/random.hpp"

#include <stdio.h>
#include <string.h>

SINGLETON_IN_CPP(NetworkLayer)

///////////////////////
// DeviceAddress

void DeviceAddress::convertFromString(const char* str)
{
	/// @todo Improve parcer to be absolutely stable
	trace << "Parsing address " << str;
	const char* pos = str;
	constexpr unsigned int tmpSize = 20;
	char tmp[tmpSize];
	for (int i=0; i<size; i++)
	{
		unsigned int cursor = 0;

		while (*pos != '\0' && *pos != '.')
			tmp[cursor++] = *pos++;

		if (cursor == tmpSize)
		{
			error << "Parsing failed: too long line";
			return;
		}

		if (pos == '\0' && i != size-1)
		{
			error << "Parsing failed: inconsistent address";
			return;
		}

		tmp[cursor] = '\0';

		address[i] = atoi(tmp);
		pos++;
	}
	trace << "Parsed: " << address[0] << "-" << address[1] << "-" << address[2];
}

///////////////////////
//OrdinaryNetworkClient

void OrdinaryNetworkClient::connect(NetworkLayer& nl)
{
    m_nl = &nl;
}

bool OrdinaryNetworkClient::isForMe(const DeviceAddress& addr)
{
    return addr == *m_address || isMyBroadcast(addr);
}

ReceivePackageCallback OrdinaryNetworkClient::getReceiver()
{
    return m_receiverCallback;
}

void OrdinaryNetworkClient::setMyAddress(const DeviceAddress& address)
{
    m_address = &address;
}

void OrdinaryNetworkClient::setPackageReceiver(ReceivePackageCallback callback)
{
    m_receiverCallback = callback;
}

void OrdinaryNetworkClient::registerMyBroadcast(const DeviceAddress& address)
{
    m_broadcasts.insert(address);
}

void OrdinaryNetworkClient::registerMyBroadcastTester(Broadcast::IBroadcastTester* tester)
{
    m_broadcastTesters.push_back(tester);
}

PackageId OrdinaryNetworkClient::send(
    DeviceAddress target,
    uint8_t* data,
    uint16_t size,
    bool waitForAck,
    PackageSendingDoneCallback doneCallback,
    PackageTimings timings
)
{
    return m_nl->send2(target, *m_address, data, size, waitForAck, doneCallback, timings);
}

bool OrdinaryNetworkClient::isMyBroadcast(const DeviceAddress& addr)
{
    if (m_broadcasts.find(addr) != m_broadcasts.end())
        return true;
    for (auto it = m_broadcastTesters.begin(); it != m_broadcastTesters.end(); it++)
        if (*it && (*it)->isAcceptableBroadcast(addr))
            return true;
    return false;
}



///////////////////////
// NetworkLayer
NetworkLayer::NetworkLayer()
{
	m_modemTask.setStackSize(800);
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

void NetworkLayer::setAddress(const DeviceAddress& address)
{
	m_selfAddress = &address;
}

void NetworkLayer::setPackageReceiver(ReceivePackageCallback callback)
{
	m_receivePackageCallback = callback;
}

void NetworkLayer::setRadioReinitCallback(RadioReinitCallback callback)
{
	m_radioReinitCallback = callback;
}

void NetworkLayer::init(IRadioPhysicalDevice* rfPhysicalDevice)
{
	Kernel::instance().assert(
			(m_selfAddress != nullptr
			&& m_receivePackageCallback != nullptr) || m_clients.size() != 0,
			"Modem initialisation fail: self address or package receiver not set"
		);
	m_rfPhysicalDevice = rfPhysicalDevice;
	m_rfPhysicalDevice->setTXDoneCallback(std::bind(&NetworkLayer::TXDoneCallback, this));
	m_rfPhysicalDevice->setDataReceiveCallback(std::bind(&NetworkLayer::RXCallback, this, std::placeholders::_1, std::placeholders::_2));
	info << "Starting m_modemTask";
	m_modemTask.run(0, 1, 1);
}

void NetworkLayer::connectClient(INetworkClient* client)
{
    client->connect(*this);
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

void NetworkLayer::enableRegularNRFReinit(bool enabled)
{
	m_regularNRFReinit = enabled;
}

void NetworkLayer::enableDebug(bool debug)
{
	m_debug = debug;
}

uint16_t NetworkLayer::generatePackageId()
{
	uint16_t id = (systemClock->getTime()) & 0xFFFF;
	if (id == 0)
		id = 1;
	return id;
}


PackageId NetworkLayer::send(
	DeviceAddress target,
	uint8_t* data,
	uint16_t size,
	bool waitForAck,
	PackageSendingDoneCallback doneCallback,
	PackageTimings timings
)
{
    return send2(target, *m_selfAddress, data, size, waitForAck, doneCallback, timings);
}

PackageId NetworkLayer::send2(
        DeviceAddress target,
        DeviceAddress sender,
        uint8_t* data,
        uint16_t size,
        bool waitForAck,
        PackageSendingDoneCallback doneCallback,
        PackageTimings timings
    )
{
    m_stager.stage("send2()");
    if (size > Package::payloadLength)
    {
        error << "Error: too long payload!";
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
        m_packages[details.packageId] = WaitingPackage();
        WaitingPackage& waitingPackage = m_packages[details.packageId];
        waitingPackage.wasCreated = time;
        waitingPackage.nextTransmission = time;
        waitingPackage.isBroadcast = broadcast.isBroadcast(target);

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
        m_packagesNoAck.push_back(Package());
        m_packagesNoAck.back().sender = sender;
        m_packagesNoAck.back().target = target;
        m_packagesNoAck.back().details = details;
        memcpy(m_packagesNoAck.back().payload, data, size);
        if (size<Package::payloadLength)
        {
            memset(m_packagesNoAck.back().payload+size, 0, Package::payloadLength-size); //< fixed third argument
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
	bool forMe = false;
	for (auto &it : m_clients)
	{
	    if (it->isForMe(received.target))
	        forMe = true;
	}

	bool oldApproachForMe = false;
	if (m_selfAddress && (received.target == *m_selfAddress || isBroadcast(received.target)))
	    oldApproachForMe = true;

	//received.target.print();
	if (!forMe && !oldApproachForMe)
		return;

	if (trace.isEnabled() && radio.isEnabled())
	{
		trace << "<== incoming package: ";
		printHex((const uint8_t*) &received, sizeof(received));
	}
	// Dispatching if this is acknoledgement
	AckPayload *ackDispatcher = reinterpret_cast<AckPayload *>(received.payload);
	if (ackDispatcher->isAck())
	{
		radio << "<== Ack for " << ackDispatcher->packageId << " from " << ADDRESS_TO_STREAM(received.sender);
		ScopedLock<Mutex> lock(m_packagesQueueMutex);
		auto it = m_packages.find(ackDispatcher->packageId);
		if (it == m_packages.end())
		{
			radio << "?=? No package with id " << ackDispatcher->packageId << " waiting ack";
			// No packages waiting this ack
			return;
		}
		radio << "|=| Ack for " << ackDispatcher->packageId << " accepted";
		if (!it->second.isBroadcast)
		{
			PackageSendingDoneCallback callback = it->second.callback;
			m_packages.erase(it);
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
		ack.sender = *m_selfAddress;
		ack.details.packageId = generatePackageId();
		memcpy(&ack.payload, &ackPayload, sizeof(ackPayload));
		memset(ack.payload+sizeof(ackPayload), 0, ack.payloadLength - sizeof(ackPayload));
		ScopedLock<Mutex> lock(m_packagesQueueMutex);
		// Adding ack package to list for sending
		m_packagesNoAck.push_back(ack);
	}

	if (checkIfIdStoredAndStore(received.details.packageId))
	{
		radio << "?=? Package with id " << received.details.packageId << " repetition detected";
	} else {
		// Putting received package to list
		m_incoming.push_back(received);
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
	if (!m_packagesNoAck.empty())
	{
		m_stager.stage("sendNext(): sending no ack");
		// If it is ack (only for output, may be removed by #ifdef DEBUG later)
		AckPayload *ackDispatcher = reinterpret_cast<AckPayload *>(m_packagesNoAck.front().payload);
		if (ackDispatcher->isAck())
		{
			radio << "==> Ack to " << ackDispatcher->packageId << " for " << ADDRESS_TO_STREAM(m_packagesNoAck.front().target) << " ";
		} else {
			radio << "==> No-ack package " << m_packagesNoAck.front().details.packageId << " for " << ADDRESS_TO_STREAM(m_packagesNoAck.front().target) << " ";
		}

		printAndSend(m_packagesNoAck.front());

		m_packagesNoAck.pop_front();
		isSendingNow = true;
		// Tell that now we are sending no-response package
		currentlySendingPackageId = 0;
		return;
	}

	Time time = systemClock->getTime();
	for (auto it=m_packages.begin(); it!=m_packages.end(); )
		// ++it in the end of cycle body for case when we removing outdated packages
	{
		// If timeout
		if (!it->second.timings.infiniteResend && time - it->second.wasCreated > it->second.timings.timeout)
		{
			PackageSendingDoneCallback callback = it->second.callback;
			uint16_t timeoutedPackageId = it->second.package.details.packageId;
			radio << "==| Package " << timeoutedPackageId << " timeouted";
			m_packages.erase(it++); // We can increment iterator
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
		bool receiverFound = false;
		for (auto &it : m_clients)
		{
		    if (it->isForMe(m_incoming.front().target))
		    {
		        it->getReceiver()
		            (m_incoming.front().sender, m_incoming.front().payload, m_incoming.front().payloadLength);
		        receiverFound = true;
		        break;
		    }
		}

		if (!receiverFound)
		{
		    if (m_receivePackageCallback)
		        m_receivePackageCallback(m_incoming.front().sender, m_incoming.front().payload, m_incoming.front().payloadLength);
		}
/*
		RCSPMultiStream answerStream;
		RCSPAggregator::instance().dispatchStream(m_incoming.front().payload, m_incoming.front().payloadLength, &answerStream);

		radio << "|=| Dispatched id " << m_incoming.front().details.packageId;
		if (!answerStream.empty())
		{
			radio << "|=| Answer for " << m_incoming.front().details.packageId << "ready";
			answerStream.send(m_incoming.front().sender, true);
		}*/
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
		trace << "payload: ";
		printHex((const uint8_t*) &package, Package::packageLength);
	}
	m_rfPhysicalDevice->sendData(Package::packageLength, (uint8_t*) &package);
}

bool NetworkLayer::stopSending(PackageId packageId)
{
	ScopedLock<Mutex> lock(m_packagesQueueMutex);
	auto it = m_packages.find(packageId);
	if (it != m_packages.end())
	{
		m_packages.erase(it);
		return true;
	}
	return false;
}

void NetworkLayer::dropAllForAddress(const DeviceAddress& address)
{
	ScopedLock<Mutex> lock(m_packagesQueueMutex);
	for(auto it = m_packages.begin(); it != m_packages.end(); )
	{
		if (it->second.package.target == address) {
			m_packages.erase(it++);
		} else {
			++it;
	}
}
}

bool NetworkLayer::updateTimeout(PackageId packageId)
{
	ScopedLock<Mutex> lock(m_packagesQueueMutex);
	auto it = m_packages.find(packageId);
	if (it != m_packages.end())
	{
		it->second.wasCreated = systemClock->getTime();
		return true;
	}
	return false;
}

/// @todo Remove this code after some time

void NetworkLayer::reinitNRF()
{
	TXDoneCallback();
	if (m_radioReinitCallback)
		m_radioReinitCallback(m_rfPhysicalDevice);
	else
		warning << "Radio reinitiallization callback was not set but it is time to reinit";
}

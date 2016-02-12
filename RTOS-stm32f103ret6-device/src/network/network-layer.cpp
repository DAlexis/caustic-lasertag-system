/*
 * package-sender.cpp
 *
 *  Created on: 28 янв. 2015 г.
 *      Author: alexey
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
// NetworkLayer
NetworkLayer::NetworkLayer()
{
	m_modemTask.setStackSize(1024);
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

void NetworkLayer::init()
{
	Kernel::instance().assert(
			m_selfAddress != nullptr
			&& m_receivePackageCallback != nullptr,
			"Modem initialisation fail: self address or package receiver not set"
		);
	nrf.setTXDoneCallback(std::bind(&NetworkLayer::TXDoneCallback, this));
	nrf.setDataReceiveCallback(std::bind(&NetworkLayer::RXCallback, this, std::placeholders::_1, std::placeholders::_2));
	nrf.init(
		IOPins->getIOPin(1, 7),
		IOPins->getIOPin(1, 12),
		IOPins->getIOPin(1, 8),
		2,
		true
	);
	//nrf.enableDebug();
	info << "Starting m_modemTask";
	m_modemTask.run(0, 1, 1);
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
		m_packages[details.packageId] = WaitingPackage();
		WaitingPackage& waitingPackage = m_packages[details.packageId];
		waitingPackage.wasCreated = time;
		waitingPackage.nextTransmission = time;
		waitingPackage.isBroadcast = broadcast.isBroadcast(target);

		waitingPackage.timings = timings;

		waitingPackage.callback = doneCallback;
		waitingPackage.package.sender = *m_selfAddress;
		waitingPackage.package.target = target;
		waitingPackage.package.details = details;
		memcpy(waitingPackage.package.payload, data, size);
		/// @todo Check package zerification!
		if (size<Package::payloadLength)
		{
			memset(&(waitingPackage.package.payload[size]), 0, Package::payloadLength-size);
		}
		//trace << "Ack-using package queued";
		return details.packageId;
	} else {
		ScopedLock<Mutex> lock(m_packagesQueueMutex);
		m_packagesNoAck.push_back(Package());
		m_packagesNoAck.back().sender = *m_selfAddress;
		m_packagesNoAck.back().target = target;
		m_packagesNoAck.back().details = details;
		memcpy(m_packagesNoAck.back().payload, data, size);
		if (size<Package::payloadLength)
		{
			memset(m_packagesNoAck.back().payload+size, 0, Package::payloadLength-size); //< fixed third argument
		}
		//trace << "No-ack package queued";
		return 0;
	}
}

void NetworkLayer::TXDoneCallback()
{
	isSendingNow = false;
}

void NetworkLayer::RXCallback(uint8_t channel, uint8_t* data)
{
	UNUSED_ARG(channel);
	Package received;
	memcpy(&received, data, sizeof(Package));

	temproraryProhibitTransmission();

	// Skipping packages for other devices
	//received.target.print();
	if (received.target != *m_selfAddress && !isBroadcast(received.target))
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
	nrf.interrogate();
	receiveIncoming();

	if (m_regularNRFReinit && (systemClock->getTime() - m_lastNRFReinitializationTime) > NRFReinitPeriod)
	{
		debug << "NRF module reinitializing to prevent broken states";
		reinitNRF();
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

		nrf.printStatus();

		//reinitNRF();
	}*/
}

void NetworkLayer::sendNext()
{
	ScopedTag tag("radio-send-next");
	if (!isTranslationAllowed())
	{
		return;
	}

	ScopedLock<Mutex> lock(m_packagesQueueMutex);
	// First, sending packages without response
	if (!m_packagesNoAck.empty())
	{
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
	for (auto it=m_packages.begin(); it!=m_packages.end(); it++)
	{
		// If timeout
		if (!it->second.timings.infiniteResend && time - it->second.wasCreated > it->second.timings.timeout)
		{
			PackageSendingDoneCallback callback = it->second.callback;
			uint16_t timeoutedPackageId = it->second.package.details.packageId;
			radio << "==| Package " << timeoutedPackageId << " timeouted";
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
			radio << "==> Need-ack package " << it->second.package.details.packageId << " for " << ADDRESS_TO_STREAM(it->second.package.target);
			//radio << "Transmitting " << it->second.package.details.packageId;
			currentlySendingPackageId = it->first;
			isSendingNow = true;
			it->second.nextTransmission = time + it->second.timings.resendTime + Random::random(it->second.timings.resendTimeDelta);

			printAndSend(it->second.package);

			return;
		}
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

		m_receivePackageCallback(m_incoming.front().sender, m_incoming.front().payload, m_incoming.front().payloadLength);
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

void NetworkLayer::temproraryProhibitTransmission()
{
	m_transmissionProhibitedTime = systemClock->getTime();
	/**
	 * This variable might be optimized. This value means time delay between last receiving and first transmission.
	 *
	 * 4000 is too small even for two devices in network: at least some acks does not reach
	 * 8000 is enough, but may be too large
	 *
	 */
	m_transmissionProhibitionPeriod = 8000;
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
	if (trace.isEnabled() && radio.isEnabled())
	{
		trace << "payload: ";
		printHex((const uint8_t*) &package, Package::packageLength);
	}
	nrf.sendData(Package::packageLength, (uint8_t*) &package);
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
	nrf.init(
		IOPins->getIOPin(1, 7),
		IOPins->getIOPin(1, 12),
		IOPins->getIOPin(1, 8),
		2,
		true
	);
}

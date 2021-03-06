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

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_

#include "rcsp/RCSP-base-types.hpp"
#include "network/network-layer-interface.hpp"
#include "hal/system-clock.hpp"
#include "utils/macro.hpp"
#include "core/os-wrappers.hpp"
#include "core/diagnostic.hpp"
#include "core/string-utils.hpp"
#include <list>
#include <map>
#include <set>

#pragma pack(push, 1)
struct PackageDetails
{
	PackageDetails(uint16_t id, uint8_t ack = 0, uint16_t ttl = 0) :
		packageId(id),
		TTL(ttl),
		needAck(ack)
	{}

	PackageDetails() :
		packageId(0),
		TTL(0),
		needAck(0)
	{
	}

	PackageId packageId;
	uint8_t TTL : 7;
	uint8_t needAck : 1;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PackageHeader
{
	DeviceAddress sender;
	DeviceAddress target;
	PackageDetails details;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Package
{
	DeviceAddress sender;
	DeviceAddress target;
	PackageDetails details;

	/// payloadLength is 23 bytes
	constexpr static uint16_t packageLength = IRadioPhysicalDevice::defaultRFPhysicalPayloadSize;
	constexpr static uint16_t payloadLength = packageLength - sizeof(sender) - sizeof(target) - sizeof(details);

	uint8_t payload[payloadLength];
};
static_assert(sizeof(Package) == Package::packageLength, "Network layer package size is bad");
#pragma pack(pop)

class NetworkLayer : public INetworkLayer
{
public:

	constexpr static uint32_t defaultTimeout = 20000000;
	constexpr static uint32_t defaultResendTime = 300000;
	constexpr static uint32_t defaultResendTimeDelta = 300000;

	NetworkLayer();
	~NetworkLayer(); //< Only for future purpose
	void start(IRadioPhysicalDevice* rfPhysicalDevice) override;
	void connectClient(INetworkClientReceiver* client) override;

	void setRadioReinitCallback(RadioReinitCallback callback) override;

    PackageId send(
        DeviceAddress target,
        DeviceAddress sender,
        const uint8_t* data,
        uint16_t size,
        bool waitForAck = false,
        PackageSendingDoneCallback doneCallback = nullptr,
        PackageTimings timings = PackageTimings(),
        INetworkClientReceiver* doNotReceiveBy = nullptr
    ) override;

    uint16_t payloadSize() override;

	bool stopSending(PackageId packageId) override;
	void dropAllForAddress(const DeviceAddress& address) override;
	bool updateTimeout(PackageId packageId) override;

	void registerBroadcast(const DeviceAddress& address) override;
	void registerBroadcastTester(Broadcast::IBroadcastTester* tester) override;

	void enableDebug(bool debug = true);

private:

	struct WaitingPackage
	{
		Time wasCreated = 0;
		Time nextTransmission = 0;
		bool isBroadcast = false;
		PackageTimings timings;
		/*
		uint32_t timeout = 0;
		uint32_t resendTime = 0;
		uint32_t resendTimeDelta = 0;
		*/
		PackageSendingDoneCallback callback;

		Package package;
	};

#pragma pack(push, 1)
	struct AckPayload
	{
		constexpr static uint16_t acknoledgementCode = 0xFFFF;
		uint8_t size = sizeof(uint16_t);
		uint16_t operationCode = acknoledgementCode;
		uint16_t packageId;
		bool isAck() { return operationCode == acknoledgementCode; }
	};
#pragma pack(pop)


	constexpr static uint32_t lastReceivedIdsBufferSize = 30;
	constexpr static uint32_t NRFReinitPeriod = 5000000;
	constexpr static uint32_t maxPackagesQueueSize = 25;

	void interrogate();
	uint16_t generatePackageId();
	INetworkClientReceiver* findClient(const DeviceAddress& target);
	void TXDoneCallback();
	void RXCallback(uint8_t channel, uint8_t* data);
	void sendNext();
	void receiveIncoming();
	bool checkIfIdStoredAndStore(uint16_t id);
	bool isTranslationAllowed();
	void temproraryProhibitTransmission(uint32_t delay);
	bool isBroadcast(const DeviceAddress& addr);
	void printAndSend(Package& package);

	bool hasFreeSpaceInQueues();

	/// @todo Remove this function after some time
	void reinitNRF();

	Time m_transmissionProhibitedTime = 0;
	Time m_transmissionProhibitionPeriod = 0;

	uint16_t currentlySendingPackageId = 0;
	bool isSendingNow = false;
	Mutex m_packagesQueueMutex;


	IRadioPhysicalDevice* m_rfPhysicalDevice = nullptr;

	std::list<Package> m_waitingPackagesNoAck;
	std::map<PackageId, WaitingPackage> m_waitingPackagesWithAck;

	std::list<Package> m_incoming;

	std::list<PackageId> m_lastReceivedIds;

	std::set<DeviceAddress> m_broadcasts;
	std::list<Broadcast::IBroadcastTester*> m_broadcastTesters;

	std::list<INetworkClientReceiver*> m_clients;

	TaskCycled m_modemTask{std::bind(&NetworkLayer::interrogate, this)};

	bool m_debug = false;
	Time m_lastNRFReinitializationTime = 0;

	RadioReinitCallback m_radioReinitCallback = nullptr;

	Stager m_stager{"Network layer", 5000000};
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_ */

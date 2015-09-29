/*
 * package-former.hpp
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_

#include "rcsp/RCSP-base-types.hpp"
#include "network/network-base-types.hpp"
#include "network/broadcast.hpp"
#include "dev/nrf24l01.hpp"
#include "hal/system-clock.hpp"
#include "utils/macro.hpp"
#include "core/os-wrappers.hpp"
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

	PackageDetails()
	{
		TTL = 0;
		needAck = 0;
	}

	PackageId packageId;
	uint8_t TTL : 7;
	uint8_t needAck : 1;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Package
{

	DeviceAddress sender;
	DeviceAddress target;
	PackageDetails details;

	/// payloadLength is 23 bytes
	constexpr static uint16_t packageLength = NRF24L01Manager::payloadSize;
	constexpr static uint16_t payloadLength = packageLength - sizeof(sender) - sizeof(target) - sizeof(details);

	uint8_t payload[payloadLength];
};
#pragma pack(pop)

class NetworkLayer
{
public:
	constexpr static uint32_t defaultTimeout = 20000000;
	constexpr static uint32_t defaultResendTime = 300000;
	constexpr static uint32_t defaultResendTimeDelta = 300000;

	NetworkLayer();
	~NetworkLayer(); //< Only for future purpose
	void init();
	void setAddress(const DeviceAddress& address);
	void setPackageReceiver(ReceivePackageCallback callback);

	/**
	 * Send package and optionaly wait for acknowledgement
	 * @param target Target device address
	 * @param data Payload
	 * @param size Payload's size
	 * @param waitForAck Need waiting for acknoledgement
	 * @param doneCallback Function to call after sending done
	 * @param timings Timings for package
	 * @return
	 */
	uint16_t send(
		DeviceAddress target,
		uint8_t* data,
		uint16_t size,
		bool waitForAck = false,
		PackageSendingDoneCallback doneCallback = nullptr,
		PackageTimings timings = PackageTimings()
	);

	void registerBroadcast(const DeviceAddress& address);
	void registerBroadcastTester(Broadcast::IBroadcastTester* tester);

	SIGLETON_IN_CLASS(NetworkLayer);
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

	void interrogate();
	uint16_t generatePackageId();
	void TXDoneCallback();
	void RXCallback(uint8_t channel, uint8_t* data);
	void sendNext();
	void receiveIncoming();
	bool checkIfIdStoredAndStore(uint16_t id);
	bool isTranslationAllowed();
	void temproraryProhibitTransmission();
	bool isBroadcast(const DeviceAddress& addr);

	Time m_transmissionProhibitedTime = 0;
	Time m_transmissionProhibitionPeriod = 0;

	uint16_t currentlySendingPackageId = 0;
	bool isSendingNow = false;
	std::list<Package> m_packagesNoAck;

	NRF24L01Manager nrf;
	std::map<uint16_t, WaitingPackage> m_packages;

	std::list<Package> m_incoming;

	std::list<uint16_t> m_lastReceivedIds;

	std::set<DeviceAddress> m_broadcasts;
	std::list<Broadcast::IBroadcastTester*> m_broadcastTesters;

	const DeviceAddress* m_selfAddress = nullptr;
	ReceivePackageCallback m_receivePackageCallback = nullptr;

	TaskCycled m_modemTask{std::bind(&NetworkLayer::interrogate, this)};
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_ */

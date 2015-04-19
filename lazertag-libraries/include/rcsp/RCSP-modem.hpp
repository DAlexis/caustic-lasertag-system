/*
 * package-former.hpp
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_

#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/operation-codes.hpp"
#include "dev/nrf24l01.hpp"
#include "hal/system-clock.hpp"
#include "core/singleton-macro.hpp"
#include <list>
#include <map>
#include <set>

using PackageId = uint16_t;

using PackageSendingDoneCallback = std::function<void(PackageId /*package id*/, bool /*was successfully sent*/)>;
using DataRXCallback = std::function<void(uint8_t* /*data*/, uint16_t dataSize)>;

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

	uint16_t packageId;
	uint8_t TTL : 7;
	uint8_t needAck : 1;
};
#pragma pack(pop)


struct DeviceAddress
{
	constexpr static uint8_t size = 3;
	uint8_t address[size];

	DeviceAddress(uint8_t a0 = 1, uint8_t a1 = 1, uint8_t a2 = 1)
		{ address[0] = a0; address[1] = a1, address[2] = a2; }

	void print() { printf("%u-%u-%u\n", address[0], address[1], address[2]); }

	void convertFromString(const char* str);

	// Operators
	inline bool operator==(const DeviceAddress& other) const
	{
		for(int i=0; i<size; i++)
			if (address[i] != other.address[i])
				return false;
			else return true;
	}

	inline bool operator!=(const DeviceAddress& other) const
	{
		return not (*this == other);
	}

	inline bool operator<(const DeviceAddress& other) const
	{
		for(int i=0; i<size; i++)
		{
			if (address[i] < other.address[i])
				return true;
			if (address[i] > other.address[i])
				return false;
		}
		return false;
	}
};

#pragma pack(push, 1)
struct Package
{

	DeviceAddress sender;
	DeviceAddress target;
	PackageDetails details;

	constexpr static uint16_t packageLength = NRF24L01Manager::payloadSize;
	constexpr static uint16_t payloadLength = packageLength - sizeof(sender) - sizeof(target) - sizeof(details);

	uint8_t payload[payloadLength];
};
#pragma pack(pop)

struct PackageTimings
{
	constexpr static uint32_t defaultTimeout = 20000000;
	constexpr static uint32_t defaultResendTime = 500000;
	constexpr static uint32_t defaultResendTimeDelta = 100000;

	PackageTimings(
			bool _infiniteResend = false,
			uint32_t _timeout = defaultTimeout,
			uint32_t _resendTime = defaultResendTime,
			uint32_t _resendTimeDelta = defaultResendTimeDelta
			) : infiniteResend(_infiniteResend), timeout(_timeout), resendTime(_resendTime), resendTimeDelta(_resendTimeDelta)
	{ }

	uint32_t timeout = defaultTimeout;
	bool infiniteResend = false;
	uint32_t resendTime = defaultResendTime;
	uint32_t resendTimeDelta = defaultResendTimeDelta;
};

class RCSPModem
{
public:
	constexpr static uint32_t defaultTimeout = 20000000;
	constexpr static uint32_t defaultResendTime = 500000;
	constexpr static uint32_t defaultResendTimeDelta = 100000;

	static RCSPModem& instance();

	/**
	 * Send package and optionaly wait for acknowledgement
	 * @param target Target device address
	 * @param data Payload
	 * @param size Payload's size
	 * @param waitForAck Need waiting for acknoledgement
	 * @param doneCallback Function to call after sending done
	 * @return
	 */
	void init();

	uint16_t send(
		DeviceAddress target,
		uint8_t* data,
		uint16_t size,
		bool waitForAck = false,
		PackageSendingDoneCallback doneCallback = nullptr,
		PackageTimings timings = PackageTimings()
	);

	void registerBroadcast(const DeviceAddress& address);

	PAR_CL(ConfigCodes::AnyDevice::Configuration, DeviceAddress, devAddr);

private:

	struct WaitingPackage
	{
		Time wasCreated = 0;
		Time nextTransmission = 0;
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
		uint8_t size = sizeof(uint16_t);
		uint16_t operationCode = ConfigCodes::acknoledgement;
		uint16_t packageId;
		bool isAck() { return operationCode == ConfigCodes::acknoledgement; }
	};
#pragma pack(pop)


	constexpr static uint32_t lastReceivedIdsBufferSize = 30;

	void interrogate();
	uint16_t generatePackageId();
	void TXDoneCallback();
	void RXCallback(uint8_t channel, uint8_t* data);
	void sendNext();
	bool checkIfIdStoredAndStore(uint16_t id);
	bool isTranslationAllowed();
	void temproraryProhibitTransmission();

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

	static RCSPModem* m_RCSPModem;
	STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_ */

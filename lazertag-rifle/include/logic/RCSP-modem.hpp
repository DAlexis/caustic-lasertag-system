/*
 * package-former.hpp
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_

#include "logic/RCSP-aggregator.hpp"
#include "logic/operation-codes.hpp"
#include "dev/nrf24l01.hpp"
#include "hal/system-clock.hpp"
#include "core/singleton-macro.hpp"
#include <list>


#define ADDRESS_LENGTH          3

using PackageSendingDoneCallback = std::function<void(uint16_t /*package id*/, bool /*was successfully sent*/)>;
using DataRXCallback = std::function<void(uint8_t* /*data*/, uint16_t dataSize)>;

#pragma pack(push, 1)
struct PackageIdAndTTL
{
	PackageIdAndTTL(uint16_t id, uint16_t ttl = 0) :
		packageId(id),
		TTL(ttl)
	{}

	PackageIdAndTTL()
	{
		TTL = 0;
	}

	uint16_t packageId : 14;
	uint16_t TTL : 2;
};
#pragma pack(pop)


struct DeviceAddress
{
	uint8_t address[ADDRESS_LENGTH];

	// Operators
	inline bool operator==(const DeviceAddress& other) const
	{
		for(int i=0; i<ADDRESS_LENGTH; i++)
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
		for(int i=0; i<ADDRESS_LENGTH; i++)
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
	PackageIdAndTTL idAndTTL;

	constexpr static uint16_t packageLength = PAYLOAD_SIZE;
	constexpr static uint16_t payloadLength = packageLength - sizeof(sender) - sizeof(target) - sizeof(idAndTTL);

	uint8_t payload[payloadLength];
};
#pragma pack(pop)



class RCSPModem
{
public:
	constexpr static uint32_t timeout = 20000000;
	constexpr static uint32_t resendTime = 500000;
	constexpr static uint32_t resendTimeDelta = 100000;

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
	uint16_t send(DeviceAddress target, uint8_t* data, uint16_t size, bool waitForAck = false, PackageSendingDoneCallback doneCallback = nullptr);

	DeviceAddress self{{1,1,1}};
private:

	struct WaitingPackage
	{
		Time wasCreated = 0;
		Time nextTransmission = 0;
		PackageSendingDoneCallback callback;

		Package package;
	};

#pragma pack(push, 1)
	struct AckPayload
	{
		uint8_t size = sizeof(uint16_t);
		uint16_t operationCode = ConfigCodes::acknoledgement;
		uint16_t packageId : 14;
		bool isAck() { return operationCode == ConfigCodes::acknoledgement; }
	};
#pragma pack(pop)


	void interrogate();
	uint16_t generatePackageId();
	void TXDoneCallback();
	void RXCallback(uint8_t channel, uint8_t* data);
	void sendNext();


	uint16_t currentlySendingPackageId = 0;
	bool isSendingNow = false;
	std::list<Package> m_packagesNoAck;

	NRF24L01Manager nrf;
	std::map<uint16_t, WaitingPackage> m_packages;

	std::list<Package> m_incoming;

	static RCSPModem* m_RCSPModem;
	STATIC_DEINITIALIZER_IN_CLASS_DECLARATION;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_ */

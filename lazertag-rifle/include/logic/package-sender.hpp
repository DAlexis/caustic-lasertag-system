/*
 * package-former.hpp
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_

#include "logic/device.hpp"
#include "logic/config-codes.hpp"
#include "dev/nrf24l01.hpp"
#include "hal/system-clock.hpp"
#include <list>

using PackageSendingDoneCallback = std::function<void(uint16_t /*package id*/, bool /*was successfully sent*/)>;
using DataRXCallback = std::function<void(uint8_t* /*data*/, uint16_t dataSize)>;

#pragma pack(push, 1)
struct PackageIdAndTTL
{
	PackageIdAndTTL()
	{
		TTL = 0;
	}

	uint16_t packageId : 14;
	uint16_t TTL : 2;
};
#pragma pack(pop)

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

class PackageSender
{
public:
	constexpr static uint32_t timeout = 20000000;
	constexpr static uint32_t resendTime = 500000;
	constexpr static uint32_t resendTimeDelta = 100000;


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
	void interrogate();
	uint16_t send(DeviceAddress target, uint8_t* data, uint16_t size, bool waitForAck = false, PackageSendingDoneCallback doneCallback = nullptr);

	DeviceAddress self{{1,2,3}};
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
};








#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PACKAGE_FORMER_HPP_ */

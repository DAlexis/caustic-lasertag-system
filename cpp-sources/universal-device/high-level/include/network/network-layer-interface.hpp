/*
 * network-layer-interface.hpp
 *
 *  Created on: 11 окт. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_LAYER_INTERFACE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_LAYER_INTERFACE_HPP_

#include "network/network-base-types.hpp"
#include "network/broadcast.hpp"
#include "network/radio-physical.hpp"

class INetworkClient;

class INetworkLayer
{
public:
	using RadioReinitCallback = std::function<void(IRadioPhysicalDevice* rfPhysicalDevice)>;
	virtual ~INetworkLayer() {}
	virtual void init(IRadioPhysicalDevice* rfPhysicalDevice) = 0;
	virtual void connectClient(INetworkClient* client) = 0;

	virtual void setRadioReinitCallback(RadioReinitCallback callback) = 0;

	/**
	 * Send package and optionaly wait for acknowledgement
	 * @param target Target device address
	 * @param data Payload
	 * @param size Payload's size
	 * @param waitForAck Need waiting for acknoledgement
	 * @param doneCallback Function to call after sending done
	 * @param timings Timings for package
	 * @param doNotReceiveBy Local network client that should not receive this package
	 * @return
	 */
	virtual PackageId send(
		DeviceAddress target,
		DeviceAddress sender,
		uint8_t* data,
		uint16_t size,
		bool waitForAck = false,
		PackageSendingDoneCallback doneCallback = nullptr,
		PackageTimings timings = PackageTimings(),
		INetworkClient* doNotReceiveBy = nullptr
	) = 0;

	virtual bool stopSending(PackageId packageId) = 0;
	virtual void dropAllForAddress(const DeviceAddress& address) = 0;
	virtual bool updateTimeout(PackageId packageId) = 0;

	virtual void registerBroadcast(const DeviceAddress& address) = 0;
	virtual void registerBroadcastTester(Broadcast::IBroadcastTester* tester) = 0;
};






#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_LAYER_INTERFACE_HPP_ */

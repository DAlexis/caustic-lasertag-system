/*
 * network-client-interface.hpp
 *
 *  Created on: 11 oct. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_CLIENT_INTERFACE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_CLIENT_INTERFACE_HPP_

#include "network/network-base-types.hpp"
#include "network/broadcast.hpp"

class INetworkLayer;

class INetworkClientReceiver;

/**
 * Interfaces collaboration scheme:
 * IRadioPhysicalDevice <- INetworkLayer -> INetworkClient1 -> IPackageReceiver1
 *                                       -> INetworkClient2 -> IPackageReceiver2
 */

/**
 * This class represents end point for network packages, so
 */
class IPayloadReceiver
{
public:
    virtual ~IPayloadReceiver() {}
    virtual void receive(DeviceAddress sender, const uint8_t* payload, uint16_t payloadLength) = 0;
};

/**
 * This interface represents network participant for lower layers.
 * It accept network addresses and receive packages
 */
class INetworkClientReceiver
{
public:
    virtual ~INetworkClientReceiver() {}
    virtual void connectNetworkLayer(INetworkLayer* nl) = 0;
    virtual void connectPayloadReceiver(IPayloadReceiver* receiver) = 0;

    /// Test if device address is acceptable
    virtual bool isForMe(const DeviceAddress& addr) = 0;

    /// Returns address for ack sending from
    virtual const DeviceAddress* mainBackAddress() = 0;

    /// Returns callback that will process given payload of package, accepted by isForMe()
    virtual void receive(DeviceAddress sender, const uint8_t* payload, uint16_t payloadLength) = 0;
};

/**
 * This interface represents network participant for upper layers.
 * It send payload to network
 */
class INetworkClientSender
{
public:
	virtual ~INetworkClientSender() {}

	/// Send package using connected network layer
	virtual PackageId send(
		DeviceAddress target,
		const uint8_t* data,
		uint16_t size,
		bool waitForAck = false,
		PackageSendingDoneCallback doneCallback = nullptr,
		PackageTimings timings = PackageTimings()
	) = 0;

	virtual uint16_t payloadSize() = 0;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_CLIENT_INTERFACE_HPP_ */

/*
 * any-device-nase.hpp
 *
 *  Created on: 18 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_DEVICE_BASE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_DEVICE_BASE_HPP_

#include "any-device/any-device-interface.hpp"
#include "any-device/device.hpp"
#include "network/network-client-interface.hpp"
#include "network/network-layer-interface.hpp"
#include "rcsp/stream.hpp"
#include "rcsp/state-saver.hpp"

/**
 * Device classes hierarchy:
 *
 * AnyDeviceBase <- Repeater
 * AnyDeviceBase <- AnyONCDeviceBase <- BluetoothBridge
 * AnyDeviceBase <- AnyONCDeviceBase <- AnyRCSPClientDeviceBase <- Rifle
 * AnyDeviceBase <- AnyONCDeviceBase <- AnyRCSPClientDeviceBase <- Head Sensor
 * AnyDeviceBase <- AnyONCDeviceBase <- AnyRCSPClientDeviceBase <- Smart Point
 * AnyDeviceBase <- AnyONCDeviceBase <- AnyRCSPClientDeviceBase <- ... other playable devices
 *
 */

/**
 * Any Caustic Lasertag system device derive this class
 */
class AnyDeviceBase : public IAnyDevice
{
public:
    AnyDeviceBase();
    void assignExistingNetworkLayer(INetworkLayer* existingNetworkLayer);

    DeviceConfiguration deviceConfig;

protected:
    void initNetwork();

    INetworkLayer *m_networkLayer = nullptr;
    RCSPAggregator* m_aggregator;
};


#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_DEVICE_BASE_HPP_ */

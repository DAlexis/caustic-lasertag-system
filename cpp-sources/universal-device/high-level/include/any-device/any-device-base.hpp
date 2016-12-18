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
#include "network/network-client.hpp"
#include "rcsp/state-saver.hpp"

class AnyDeviceBase : public IAnyDevice
{
public:
    AnyDeviceBase();

    DeviceConfiguration deviceConfig;

protected:

    OrdinaryNetworkClient m_networkClient;
    RCSPAggregator* m_aggregator;
    MainStateSaver m_stateSaver;
};




#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_DEVICE_BASE_HPP_ */

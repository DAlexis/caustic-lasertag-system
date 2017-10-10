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
#include "network/network-layer-interface.hpp"
#include "rcsp/stream.hpp"
#include "rcsp/state-saver.hpp"

class AnyDeviceBase : public IAnyDevice
{
public:
    AnyDeviceBase();

    DeviceConfiguration deviceConfig;

protected:
    virtual void initNetworkClient();
    void initNetwork();

    INetworkLayer *m_networkLayer = nullptr;
    OrdinaryNetworkClient m_networkClient;

    RCSPAggregator* m_aggregator;
    MainStateSaver m_stateSaver;

private:
    void createNetworkLayerIfEmpty();
};

class AnyRCSPClientDeviceBase : public AnyDeviceBase
{
protected:
    void initNetworkClient() override;
    RCSPNetworkListener m_networkPackagesListener{&RCSPAggregator::getActiveAggregator()};

};



#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_DEVICE_BASE_HPP_ */

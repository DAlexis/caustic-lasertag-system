/*
 * network-client.hpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_CLIENT_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_CLIENT_HPP_

#include "network/network-base-types.hpp"
#include "network/broadcast.hpp"

#include <set>
#include <list>

class NetworkLayer;

class INetworkClient;

class IPackageReceiver
{
public:
    virtual ~IPackageReceiver() {}
    virtual void receivePackage(DeviceAddress sender, uint8_t* payload, uint16_t payloadLength) = 0;
    virtual void connectClient(INetworkClient* client) = 0;
};

/**
 * This class represents one network participant that can:
 * - accept network addresses and receive package
 * - send network packages with concrete sender address
 */
class INetworkClient
{
public:
    virtual ~INetworkClient() {}

    virtual void connectNetworkLayer(NetworkLayer& nl) = 0;
    virtual void connectPackageReceiver(IPackageReceiver* receiver) = 0;

    /// Test if device address is acceptable
    virtual bool isForMe(const DeviceAddress& addr) = 0;

    /// Returns address for ack sending from
    virtual const DeviceAddress* mainBackAddress() = 0;

    /// Returns callback that will process given payload of package, accepted by isForMe()
    virtual ReceivePackageCallback getReceiver() = 0;

    /// Send package using connected network layer
    virtual PackageId send(
        DeviceAddress target,
        uint8_t* data,
        uint16_t size,
        bool waitForAck = false,
        PackageSendingDoneCallback doneCallback = nullptr,
        PackageTimings timings = PackageTimings()
    ) = 0;
};


class OrdinaryNetworkClient : public INetworkClient
{
public:
    void connectNetworkLayer(NetworkLayer& nl) override;
    bool isForMe(const DeviceAddress& addr) override;
    ReceivePackageCallback getReceiver() override;
    const DeviceAddress* mainBackAddress() override;
    void connectPackageReceiver(IPackageReceiver* receiver) override;

    void setMyAddress(const DeviceAddress& address);
    void registerMyBroadcast(const DeviceAddress& address);
    void registerMyBroadcastTester(Broadcast::IBroadcastTester* tester);

    void setSendingToMyself(bool sendToMyself);

    virtual PackageId send(
        DeviceAddress target,
        uint8_t* data,
        uint16_t size,
        bool waitForAck = false,
        PackageSendingDoneCallback doneCallback = nullptr,
        PackageTimings timings = PackageTimings()
    ) override;

private:
    bool isMyBroadcast(const DeviceAddress& addr);
    NetworkLayer* m_nl = nullptr;
    IPackageReceiver* m_packageReceiver = nullptr;
    bool m_sendToMyself = false;

    const DeviceAddress* m_address = nullptr;

    std::set<DeviceAddress> m_broadcasts;
    std::list<Broadcast::IBroadcastTester*> m_broadcastTesters;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_CLIENT_HPP_ */

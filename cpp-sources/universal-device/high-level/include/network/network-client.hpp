/*
 * network-client.hpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_CLIENT_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_CLIENT_HPP_

#include "network/network-client-interface.hpp"

#include <set>
#include <list>

class OrdinaryNetworkClient : public INetworkClient
{
public:
    void connectNetworkLayer(INetworkLayer* nl) override;
    bool isForMe(const DeviceAddress& addr) override;
    void receive(DeviceAddress sender, uint8_t* payload, uint16_t payloadLength) override;
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
    INetworkLayer* m_nl = nullptr;
    IPackageReceiver* m_packageReceiver = nullptr;
    bool m_sendToMyself = false;

    const DeviceAddress* m_address = nullptr;

    std::set<DeviceAddress> m_broadcasts;
    std::list<Broadcast::IBroadcastTester*> m_broadcastTesters;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_NETWORK_NETWORK_CLIENT_HPP_ */

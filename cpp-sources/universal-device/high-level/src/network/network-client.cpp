/*
 * network-client.cpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#include "network/network-client.hpp"
#include "network/network-layer-interface.hpp"
#include "core/logging.hpp"

///////////////////////
//OrdinaryNetworkClient

void OrdinaryNetworkClient::connectNetworkLayer(INetworkLayer* nl)
{
    m_nl = nl;
}

bool OrdinaryNetworkClient::isForMe(const DeviceAddress& addr)
{
    return addr == *m_address || isMyBroadcast(addr);
}

ReceivePackageCallback OrdinaryNetworkClient::getReceiver()
{
   return [this] (DeviceAddress sender, uint8_t* payload, uint16_t payloadLength) {
            m_packageReceiver->receivePackage(sender, payload, payloadLength);
   };
}

const DeviceAddress* OrdinaryNetworkClient::mainBackAddress()
{
    return m_address;
}

void OrdinaryNetworkClient::setMyAddress(const DeviceAddress& address)
{
    m_address = &address;
}

void OrdinaryNetworkClient::registerMyBroadcast(const DeviceAddress& address)
{
    m_broadcasts.insert(address);
}

void OrdinaryNetworkClient::registerMyBroadcastTester(Broadcast::IBroadcastTester* tester)
{
    m_broadcastTesters.push_back(tester);
}

void OrdinaryNetworkClient::connectPackageReceiver(IPackageReceiver* receiver)
{
    m_packageReceiver = receiver;
    m_packageReceiver->connectClient(this);
}

void OrdinaryNetworkClient::setSendingToMyself(bool sendToMyself)
{
    m_sendToMyself = sendToMyself;
}

PackageId OrdinaryNetworkClient::send(
    DeviceAddress target,
    uint8_t* data,
    uint16_t size,
    bool waitForAck,
    PackageSendingDoneCallback doneCallback,
    PackageTimings timings
)
{
    if (m_address == nullptr)
    {
        error << "OrdinaryNetworkClient error: m_address == nullptr";
        return 0;
    }
    if (m_sendToMyself)
        return m_nl->send(target, *m_address, data, size, waitForAck, doneCallback, timings);
    else
        return m_nl->send(target, *m_address, data, size, waitForAck, doneCallback, timings, this);
}

bool OrdinaryNetworkClient::isMyBroadcast(const DeviceAddress& addr)
{
    if (m_broadcasts.find(addr) != m_broadcasts.end())
        return true;
    for (auto it = m_broadcastTesters.begin(); it != m_broadcastTesters.end(); it++)
        if (*it && (*it)->isAcceptableBroadcast(addr))
            return true;
    return false;
}

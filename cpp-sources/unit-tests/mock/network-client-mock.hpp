/*
 * network-client-mock.hpp
 *
 *  Created on: 12 нояб. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIT_TESTS_MOCK_NETWORK_CLIENT_MOCK_HPP_
#define UNIT_TESTS_MOCK_NETWORK_CLIENT_MOCK_HPP_

#include "network/network-client-interface.hpp"
#include "core/buffer.hpp"

class NetworkClientMock : public INetworkClientReceiver
{
public:
    void connectNetworkLayer(INetworkLayer* nl) override;
    bool isForMe(const DeviceAddress& addr) override;
    void receive(DeviceAddress sender, const uint8_t* payload, uint16_t payloadLength) override;
    const DeviceAddress* mainBackAddress() override;
    void connectPayloadReceiver(IPayloadReceiver* receiver) override;

    PackageId send(
        DeviceAddress target,
        const uint8_t* data,
        uint16_t size,
        bool waitForAck = false,
        PackageSendingDoneCallback doneCallback = nullptr,
        PackageTimings timings = PackageTimings()
    ) override;

    uint16_t payloadSize() override;

private:
    bool isMyBroadcast(const DeviceAddress& addr);
    INetworkLayer* m_nl = nullptr;
    IPayloadReceiver* m_packageReceiver = nullptr;
    bool m_sendToMyself = false;

    const DeviceAddress* m_address = nullptr;

    std::set<DeviceAddress> m_broadcasts;
    std::list<Broadcast::IBroadcastTester*> m_broadcastTesters;
};







#endif /* UNIT_TESTS_MOCK_NETWORK_CLIENT_MOCK_HPP_ */

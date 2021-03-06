/*
 * network-layer-moc.hpp
 *
 *  Created on: 17 окт. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIT_TESTS_NETWORK_LAYER_MOCK_HPP_
#define UNIT_TESTS_NETWORK_LAYER_MOCK_HPP_

#include "network/network-layer-interface.hpp"
#include "core/buffer.hpp"
#include <vector>

class NetworkLayerMock : public INetworkLayer
{
public:
	void start(IRadioPhysicalDevice* rfPhysicalDevice) override;
	void connectClient(INetworkClientReceiver* client) override;

	void setRadioReinitCallback(RadioReinitCallback callback) override;

	PackageId send(
		DeviceAddress target,
		DeviceAddress sender,
		const uint8_t* data,
		uint16_t size,
		bool waitForAck = false,
		PackageSendingDoneCallback doneCallback = nullptr,
		PackageTimings timings = PackageTimings(),
		INetworkClientReceiver* doNotReceiveBy = nullptr
	) override;

	uint16_t payloadSize() override;

	bool stopSending(PackageId packageId) override;
	void dropAllForAddress(const DeviceAddress& address) override;
	bool updateTimeout(PackageId packageId) override;

	void registerBroadcast(const DeviceAddress& address) override;
	void registerBroadcastTester(Broadcast::IBroadcastTester* tester) override;

	const Buffer& buffer();

private:
	Buffer m_sendedBuffer;
};





#endif /* UNIT_TESTS_NETWORK_LAYER_MOCK_HPP_ */

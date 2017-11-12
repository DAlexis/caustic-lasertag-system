
#include "any-device/any-device-base.hpp"
#include "any-device/any-rcsp-device-base.hpp"
#include "rcsp/aggregator.hpp"
#include "core/logging.hpp"
#include "network/network-layer.hpp"
#include "dev/nrf24l01.hpp"

AnyDeviceBase::AnyDeviceBase(INetworkLayer *existingNetworkLayer) :
	m_networkLayer(existingNetworkLayer),
	m_aggregator(new RCSPAggregator())
{
	m_powerMonitor.init();
	if (m_networkLayer == nullptr)
	{
		m_networkLayer = new NetworkLayer();
		NRF24L01Manager *nrf = new NRF24L01Manager();
		auto radioReinit = [](IRadioPhysicalDevice* rf) {
			static_cast<NRF24L01Manager*>(rf)->init(
					IOPins->getIOPin(1, 7),
					IOPins->getIOPin(1, 12),
					IOPins->getIOPin(1, 8),
					2,
					true,
					1
				);
		};
		radioReinit(nrf);
		m_networkLayer->setRadioReinitCallback(radioReinit);
		m_networkLayer->start(nrf);
	}
}

void AnyDeviceBase::assignExistingNetworkLayer(INetworkLayer* existingNetworkLayer)
{
	m_networkLayer = existingNetworkLayer;
}

///////////////////////
// AnyONCDeviceBase

AnyONCDeviceBase::AnyONCDeviceBase(INetworkLayer *existingNetworkLayer) :
		AnyDeviceBase(existingNetworkLayer)
{
	m_networkClient = new OrdinaryNetworkClient();
	static_cast<OrdinaryNetworkClient*>(m_networkClient)->setMyAddress(deviceConfig.devAddr);
	static_cast<OrdinaryNetworkClient*>(m_networkClient)->registerMyBroadcast(broadcast.any);
	m_networkLayer->connectClient(m_networkClient);
}

///////////////////////
// AnyRCSPClientDeviceBase

AnyRCSPClientDeviceBase::AnyRCSPClientDeviceBase(INetworkLayer *existingNetworkLayer) :
	AnyONCDeviceBase(existingNetworkLayer),
	m_stateSaver(m_aggregator)
{
	m_networkClient->connectPackageReceiver(&m_networkPackagesListener);
}


#include "any-device/any-device-base.hpp"
#include "rcsp/aggregator.hpp"
#include "core/logging.hpp"
#include "network/network-layer.hpp"
#include "dev/nrf24l01.hpp"

AnyDeviceBase::AnyDeviceBase() :
    m_aggregator(&RCSPAggregator::getActiveAggregator()),
    m_stateSaver(m_aggregator)
{

}

void AnyDeviceBase::initNetworkClient()
{
	createNetworkLayerIfEmpty();
    debug << "Setting address to " << ADDRESS_TO_STREAM(deviceConfig.devAddr);
    m_networkClient.setMyAddress(deviceConfig.devAddr);
    m_networkClient.registerMyBroadcast(broadcast.any);
    m_networkLayer->connectClient(&m_networkClient);
}

void AnyDeviceBase::initNetwork()
{
	createNetworkLayerIfEmpty();
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
    m_networkLayer->init(nrf);
}

void AnyDeviceBase::createNetworkLayerIfEmpty()
{
	if (m_networkLayer == nullptr)
	{
		m_networkLayer = new NetworkLayer();
#ifdef DEBUG
		reinterpret_cast<NetworkLayer*>(m_networkLayer)->enableDebug(true);
#endif
	}
}

void AnyRCSPClientDeviceBase::initNetworkClient()
{
    AnyDeviceBase::initNetworkClient();
    m_networkClient.connectPackageReceiver(&m_networkPackagesListener);
}

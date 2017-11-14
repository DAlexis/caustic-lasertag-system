/*
 * any-onc-device-base.hpp
 *
 *  Created on: 11 окт. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_ONC_DEVICE_BASE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_ONC_DEVICE_BASE_HPP_

#include "any-device/any-device-base.hpp"
#include "network/network-client-interface.hpp"

/**
 * Any Ordinary Network Client device base class.
 * RCSP client devices are derived from it
 */
class AnyONCDeviceBase : public AnyDeviceBase
{
protected:
	AnyONCDeviceBase(INetworkLayer *existingNetworkLayer = nullptr);

private:
	OrdinaryNetworkClient m_onc;

public:
	INetworkClientReceiver *m_networkClient = &m_onc;
	INetworkClientSender *m_networkClientSender = &m_onc;
};


#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_ONC_DEVICE_BASE_HPP_ */

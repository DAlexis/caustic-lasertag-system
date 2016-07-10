/*
 * smart-point.cpp
 *
 *  Created on: 9 июл. 2016 г.
 *      Author: dalexies
 */

#include "smart-point/smart-point.hpp"
#include "device/device-base-types.hpp"
#include "network/network-layer.hpp"
#include "core/logging.hpp"
#include "rcsp/RCSP-stream.hpp"

SmartPoint::SmartPoint()
{
	deviceConfig.deviceType = DeviceTypes::smartPoint;
	m_tasksPool.setStackSize(400);
}

void SmartPoint::init(const Pinout& pinout)
{
	m_tasksPool.run();
	info << "RCSP modem initialization";
	NetworkLayer::instance().setAddress(deviceConfig.devAddr);
	NetworkLayer::instance().setPackageReceiver(RCSPMultiStream::getPackageReceiver());
	NetworkLayer::instance().registerBroadcast(broadcast.any);
	NetworkLayer::instance().registerBroadcast(broadcast.smartPoint);
	NetworkLayer::instance().init();
}

void SmartPoint::setDafaultPinout(Pinout& pinout)
{

}

bool SmartPoint::checkPinout(const Pinout& pinout)
{
	/// @todo Implement here
	return true;
}

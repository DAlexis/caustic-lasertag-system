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
	info << "Wav player initialization";
	WavPlayer::instance().init();

	info << "RCSP modem initialization";
	NetworkLayer::instance().setAddress(deviceConfig.devAddr);
	NetworkLayer::instance().setPackageReceiver(RCSPMultiStream::getPackageReceiver());
	NetworkLayer::instance().registerBroadcast(broadcast.any);
	NetworkLayer::instance().registerBroadcast(broadcast.smartPoint);
	NetworkLayer::instance().init();

	initSounds();
	m_tasksPool.add(
			[this] { m_systemReadySound.play(); },
			100000,
			100000,
			1
	);

	m_tasksPool.run();

	info << "UI initialization";
	m_ui.init();

	m_tasksPool.add([this](){
			m_ui.doIteration();
		},
		50000,
		500000
	);
/*
	m_tasksPool.add([this](){
			m_keyboard.interrogate();
		},
		50000
	);*/

}

void SmartPoint::setDafaultPinout(Pinout& pinout)
{

}

bool SmartPoint::checkPinout(const Pinout& pinout)
{
	/// @todo Implement here
	return true;
}

void SmartPoint::initSounds()
{
	m_systemReadySound.readVariants("sound/startup-", ".wav", 1);
}

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

#include "ir/ir-physical-tv.hpp"
#include "ir/ir-presentation-mt2.hpp"

SmartPoint::SmartPoint()
{
	deviceConfig.deviceType = DeviceTypes::smartPoint;
	m_tasksPool.setStackSize(400);
}

void SmartPoint::init(const Pinout& pinout)
{
	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values";
		samrtPointConfig.resetToDefault();
	}

	m_MT2Interogator.setStackSize(512);

	m_irPhysicalReceiver = new IRReceiverTV;
	m_irPhysicalReceiver->setIOPin(IOPins->getIOPin(0, 0));
	m_irPhysicalReceiver->init();
	m_irPhysicalReceiver->setEnabled(true);

	m_irPresentationReceiver = new IRPresentationReceiverMT2;
	m_irPresentationReceiver->setPhysicalReceiver(m_irPhysicalReceiver);
	m_irPresentationReceiver->init();

	m_irPresentationReceiversGroup = new PresentationReceiversGroupMT2;
	m_irPresentationReceiversGroup->connectReceiver(*m_irPresentationReceiver);
	m_MT2Interogator.registerObject(m_irPresentationReceiversGroup);

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

	m_MT2Interogator.run();

	m_tasksPool.add([this](){
			m_ui.doIteration();
		},
		100000,
		500000
	);
	m_tasksPool.add([this](){
			state.ticTime();
		},
		50000,
		0
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

void SmartPoint::catchShot(ShotMessage msg)
{
	info << "** Shot from team " << msg.teamId;
	state.acitateByTeam(msg.teamId);
}

void SmartPoint::initSounds()
{
	m_systemReadySound.readVariants("sound/startup-", ".wav", 1);
}

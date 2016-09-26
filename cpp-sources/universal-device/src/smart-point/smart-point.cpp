/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#include "smart-point/smart-point.hpp"
#include "device/device-base-types.hpp"
#include "network/network-layer.hpp"
#include "core/logging.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "rcsp/RCSP-state-saver.hpp"

#include "dev/nrf24l01.hpp"

#include "ir/ir-physical-tv.hpp"
#include "ir/ir-presentation-mt2.hpp"

SmartPoint::SmartPoint()
{
	deviceConfig.deviceType = DeviceTypes::smartPoint;
	m_tasksPool.setStackSize(400);
}

void SmartPoint::init(const Pinout& pinout)
{
	info << "Loading default config";
	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values";
		samrtPointConfig.resetToDefault();
	}

	info << "Restoring state";
	MainStateSaver::instance().setFilename("state-save");
	/// @todo Chack that rife is turned on/off correctly anway
	if (MainStateSaver::instance().tryRestore())
	{
		info  << "  restored";
	} else {
		warning << "  restoring failed";
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
	NetworkLayer::instance().setRadioReinitCallback(radioReinit);
	NetworkLayer::instance().init(nrf);

	initSounds();
	m_tasksPool.add(
			[this] { m_systemReadySound.play(); },
			100000,
			100000,
			1
	);

	MainStateSaver::instance().runSaver(8000);

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

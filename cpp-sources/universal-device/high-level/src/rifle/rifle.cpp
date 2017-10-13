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


#include "core/logging.hpp"
#include "core/power-monitor.hpp"
#include "core/string-utils.hpp"
#include "dev/io-pins-utils.hpp"
#include "dev/nrf24l01.hpp"
#include "dev/wav-player.hpp"
#include "hal/i2c.hpp"
#include "hal/system-clock.hpp"
#include "ir/ir-physical-tv.hpp"
#include "ir/ir-presentation-mt2.hpp"
#include "network/broadcast.hpp"
#include "rcsp/state-saver.hpp"
#include "rcsp/stream.hpp"
#include "rifle/resources.hpp"
#include "rifle/rifle.hpp"
#include "rifle/rifle-display-lcd5110.hpp"
#include "rifle/rifle-display-ssd1306.hpp"

#include <stdio.h>
#include <string>
#include <utility>

Rifle::Rifle()
{
	deviceConfig.deviceType = DeviceTypes::rifle;
	m_tasksPool.setStackSize(400);
}

void Rifle::setDefaultPinout(Pinout& pinout)
{
	pinout.set(PinoutTexts::trigger, 0, 0);
	pinout.set(PinoutTexts::reload, 1, 2);
	pinout.set(PinoutTexts::automatic, 2, 7);
	pinout.set(PinoutTexts::semiAutomatic, 1, 9);
	pinout.set(PinoutTexts::mag1Sensor, 0, 11);
	pinout.set(PinoutTexts::mag2Sensor, 0, 12);
	pinout.set(PinoutTexts::flash, 1, 11);
	pinout.set(PinoutTexts::vibro, 0, 1);
}

bool Rifle::checkPinout(const Pinout& pinout)
{
	bool result = true;
	if (!pinout[PinoutTexts::trigger].exists())
	{
		error << "Trigger pin is not set";
		result = false;
	}

	if (!pinout[PinoutTexts::reload].exists())
	{
		error << "Reload pin is not set";
		result = false;
	}

	if (!pinout[PinoutTexts::automatic].exists())
	{
		error << "Automatic switch pin is not set";
		result = false;
	}

	if (!pinout[PinoutTexts::semiAutomatic].exists())
	{
		error << "Semi-automatic switch pin is not set";
		result = false;
	}

	return result;
}


void Rifle::init(const Pinout& pinout, bool isSdcardOk)
{
	if (!isSdcardOk)
	{
		error << "Fatal error: rifle cannot operate without sdcard!";
	}
	info << "Wav player initialization";
	WavPlayer::instance().init();

	// Power monitor should be initialized before configuration reading
	PowerMonitor::instance().init();

	info << "Loading default config";
	m_aggregator->readIni("config.ini");

	info << "Restoring state";
	m_stateSaver.setFilename("state-save");
	/// @todo Chack that rife is turned on/off correctly anway
	if (m_stateSaver.tryRestore())
	{
		info  << "  restored";
	} else {
		warning << "  restoring failed";
		rifleReset();
	}
/*
	m_tasksPool.add(
			[this]() {
				playerState.print();
				updatePlayerState();
			},
			5000000
	);*/

	m_tasksPool.add(
			[this] { PowerMonitor::instance().interrogate(); },
			100000
	);

	m_tasksPool.add(
			[this] { sendHeartbeatToHS(); },
			heartbeatPeriod
	);

	m_tasksPool.add(
			[this] { m_systemReadySound.play(); },
			100000,
			100000,
			1
	);

	info << "Configuring buttons";
	//info << "Current pinout:";
	//pinout.printPinout();
	m_fireButton = ButtonsPool::instance().getButtonManager(
			pinout[PinoutTexts::trigger].port,
			pinout[PinoutTexts::trigger].pin
	);
	m_fireButton->useEXTI(false);
	m_fireButton->setPressedState(pinout[PinoutTexts::trigger].inverted);
	m_fireButton->setAutoRepeat(config.automaticAllowed);
	m_fireButton->setRepeatPeriod(config.firePeriod);
	m_fireButton->setCallback(std::bind(&Rifle::makeShot, this, std::placeholders::_1));
	m_fireButton->turnOn();

	m_buttonsInterrogator.registerObject(m_fireButton);

	m_reloadButton = ButtonsPool::instance().getButtonManager(
			pinout[PinoutTexts::reload].port,
			pinout[PinoutTexts::reload].pin
	);
	m_reloadButton->setAutoRepeat(false);
	m_reloadButton->setPressedState(pinout[PinoutTexts::reload].inverted);
	m_reloadButton->setRepeatPeriod(config.firePeriod);
	m_reloadButton->setCallback(std::bind(&Rifle::distortBolt, this, std::placeholders::_1));
	m_reloadButton->turnOn();

	m_buttonsInterrogator.registerObject(m_reloadButton);

	m_automaticFireSwitch = ButtonsPool::instance().getButtonManager(
			pinout[PinoutTexts::automatic].port,
			pinout[PinoutTexts::automatic].pin
	);
	m_automaticFireSwitch->setPressedState(pinout[PinoutTexts::automatic].inverted);
	m_automaticFireSwitch->turnOff();

	m_semiAutomaticFireSwitch = ButtonsPool::instance().getButtonManager(
			pinout[PinoutTexts::semiAutomatic].port,
			pinout[PinoutTexts::semiAutomatic].pin
	);
	m_semiAutomaticFireSwitch->setPressedState(pinout[PinoutTexts::semiAutomatic].inverted);
	m_semiAutomaticFireSwitch->turnOff();

	if (pinout[PinoutTexts::flash].exists())
	{
		m_fireFlash = IOPins->getIOPin(pinout[PinoutTexts::flash].port, pinout[PinoutTexts::flash].pin);
		m_fireFlash->switchToOutput();
		m_fireFlash->reset();
	}
	if (pinout[PinoutTexts::vibro].exists())
	{
		m_vibroEngine = IOPins->getIOPin(pinout[PinoutTexts::vibro].port, pinout[PinoutTexts::vibro].pin);
		m_vibroEngine->switchToOutput();
		m_vibroEngine->reset();
	}

	if (!config.isAutoReloading() && !config.isReloadingByDistortingTheBolt())
	{
		m_magazine1Sensor = ButtonsPool::instance().getButtonManager(
				pinout[PinoutTexts::mag1Sensor].port,
				pinout[PinoutTexts::mag1Sensor].pin
		);
		m_magazine1Sensor->setAutoRepeat(false);
		m_magazine1Sensor->setPressedState(pinout[PinoutTexts::mag1Sensor].inverted);
		m_magazine1Sensor->setRepeatPeriod(config.firePeriod);
		m_magazine1Sensor->setCallback(std::bind(&Rifle::magazineSensor, this, true, 1, std::placeholders::_1));
		m_magazine1Sensor->setDepressCallback(std::bind(&Rifle::magazineSensor, this, false, 1, true));
		m_magazine1Sensor->turnOn();
		m_buttonsInterrogator.registerObject(m_magazine1Sensor);

		m_magazine2Sensor = ButtonsPool::instance().getButtonManager(
				pinout[PinoutTexts::mag2Sensor].port,
				pinout[PinoutTexts::mag2Sensor].pin
		);
		m_magazine2Sensor->setAutoRepeat(false);
		m_magazine2Sensor->setPressedState(pinout[PinoutTexts::mag2Sensor].inverted);
		m_magazine2Sensor->setRepeatPeriod(config.firePeriod);
		m_magazine2Sensor->setCallback(std::bind(&Rifle::magazineSensor, this, true, 2, std::placeholders::_1));
		m_magazine2Sensor->setDepressCallback(std::bind(&Rifle::magazineSensor, this, false, 2, true));
		m_magazine2Sensor->turnOn();
		m_buttonsInterrogator.registerObject(m_magazine2Sensor);
	}
	detectReloadAndMagsState();

	initSequenceDetectors();

	m_buttonsInterrogator.setStackSize(512);
	m_buttonsInterrogator.run(10);


	info << "Configuring MT2 transmitter";

	m_irPhysicalTransmitter = new IRTransmitterTV();
	m_irPhysicalTransmitter->setFireEmitter(fireEmittersPool->get());
	m_irPhysicalTransmitter->setPower(config.outputPower);
	m_irPhysicalTransmitter->init(pinout);

	m_irPresentationTransmitter = new IRPresentationTransmitterMT2(*m_aggregator);
	m_irPresentationTransmitter->setPhysicalTransmitter(m_irPhysicalTransmitter);
	m_irPresentationTransmitter->init();

	info << "Looking for sound files...";
	initSounds();

	info << "Network and RCSP initialization";
	initNetwork();
	initNetworkClient();
    static_cast<OrdinaryNetworkClient*>(m_networkClient)->registerMyBroadcast(broadcast.anyGameDevice);
    static_cast<OrdinaryNetworkClient*>(m_networkClient)->registerMyBroadcast(broadcast.rifles);

	info << "Other initialization";
	rifleTurnOff();

	m_tasksPool.add(
		[this](){ checkHeartBeat(); },
		maxNoHeartbeatDelay,
		maxNoHeartbeatDelay
	);

	m_tasksPool.add(
			[this] { m_rfidController.interrogate(); },
			100000
	);

	//updatePlayerState();

	m_stateSaver.runSaver(8000);
	m_tasksPool.run();

	m_mfrcWrapper.init();
	m_rfidController.setCallback([this](RifleRFIDController::Mode mode){ cardOperationDoneCallback(mode); });

	rfidSwitchToRead();

	//m_rfidController.writeServiceCard();
	//state.rfidState = RifleState::RFIDSate::programMasterCard;

	info << "Display initialization";
	auto pinoutRes = pinout.getParameter("display");
	if (pinoutRes)
	{
        if (pinoutRes.details == "lcd5110")
        {
            info << "Creating lcd 5110 display controller";
            m_display = new RifleLCD5110Display;
        } else if (pinoutRes.details == "ssd1306")
        {
            info << "Creating ssd1306 display controller";
            m_display = new RifleSSD1306Display;
        }
	}

	if (m_display)
	{
	    m_display->setData(&rifleOwner, &state, &playerState);
	    if (m_display->init())
	    {
            m_tasksPool.add([this](){
                    m_display->update();
                },
                500000
            );
	    } else {
	        delete m_display;
	    }
	}

	info << "Rifle ready to use\n";
}

void Rifle::detectReloadAndMagsState()
{
	if (config.isAutoReloading() || config.isReloadingByDistortingTheBolt())
	{
		m_state = WeaponState::ready;
	} else {
		m_currentMagazineNumber = getCurrentMagazineNumber();
		info << "Magazine inserted: " << m_currentMagazineNumber;
		if (m_currentMagazineNumber != 0)
		{
			m_state = WeaponState::ready;
		} else {
			m_state = WeaponState::magazineRemoved;
			m_fireButton->setAutoRepeat(false);
		}
	}
}

bool Rifle::isCommandFromActualHS()
{
	return !m_networkPackagesListener.hasSender() || m_networkPackagesListener.sender() == config.headSensorAddr;
}

void Rifle::loadConfig()
{
	IniParser *parcer = new IniParser;
	parcer->setCallback([](const char* group, const char* key, const char* val) { UNUSED_ARG(group); printf("k = %s, v = %s\n", key, val); });
	Result res = parcer->parseFile("default-config.ini");
	if (!res.isSuccess)
		printf("Error: %s\n", res.errorText);
	delete parcer;
}

void Rifle::initSounds()
{
	m_systemReadySound.readVariants("sound/startup-", ".wav", 1);
	m_RFIDCardReaded.readVariants("sound/rfid-readed-", ".wav", 1);
	m_connectedToHeadSensorSound.readVariants("sound/connected-", ".wav", 0);
	m_shootingSound.readVariants("sound/shoot-", ".wav", 0);
	m_reloadingSound.readVariants("sound/reload-", ".wav", 0);
	m_noAmmoSound.readVariants("sound/no-ammo-", ".wav", 0);
	m_noMagazines.readVariants("sound/no-magazines-", ".wav", 0);
	m_respawnSound.readVariants("sound/respawn-", ".wav", 1);
	m_dieSound.readVariants("sound/die-", ".wav", 1);
	m_woundSound.readVariants("sound/wound-", ".wav", 1);
	m_enemyDamaged.readVariants("sound/enemy-injured-", ".wav", 1);
	m_enemyKilled.readVariants("sound/enemy-killed-", ".wav", 1);
	m_friendDamaged.readVariants("sound/friend-injured-", ".wav", 1);
	m_noHeartbeat.readVariants("sound/no-heartbeat-", ".wav", 1);
	m_noShockedShooting.readVariants("sound/shocked-shooting", ".wav", 0);
	m_hsSwitchRejected.readVariants("sound/hs-switch-rejected", ".wav", 0);
	m_rfidProgrammingInProcess.readVariants("sound/rfid-programming", ".wav", 0);
}

void Rifle::initSequenceDetectors()
{
	m_writeCardSeqDetector.addKeyToSequence(m_reloadButton);
	m_writeCardSeqDetector.addKeyToSequence(m_reloadButton);
	m_writeCardSeqDetector.addKeyToSequence(m_reloadButton);
	m_writeCardSeqDetector.setCallback([this]() { rifleRFIDProgramHSAddr(); } );
	m_buttonsInterrogator.registerObject(&m_writeCardSeqDetector);
}

void Rifle::rfidSwitchToRead()
{
	m_rfidController.readCommands();
	state.rfidState = RifleState::RFIDSate::reading;
}

void Rifle::makeShot(bool isFirst)
{
	switch(m_state)
	{
	case WeaponState::magazineReturned:
	case WeaponState::ready:
		if (isSafeSwitchSelected())
			break;

		if (isShocked())
		{
			info << "Player shocked!";
			m_noShockedShooting.play();
			break;
		}

		if (!isFirst && !config.automaticAllowed)
			break;

		if (state.bulletsInMagazineCurrent != 0)
		{
			state.bulletsInMagazineCurrent--;
			info << "<----<< Sending bullet with damage " << config.damageMin;

			prepareAndSendShotMsg();

			if (m_fireFlash)
			{
				m_fireFlash->set();
				delayedSwitchPin(m_tasksPool, m_fireFlash, false, config.fireFlashPeriod);
			}
			if (m_vibroEngine)
			{
				m_vibroEngine->set();
				delayedSwitchPin(m_tasksPool, m_vibroEngine, false, config.fireVibroPeriod);
			}
			m_shootingSound.play();
			if (state.bulletsInMagazineCurrent == 0)
			{
				m_state = WeaponState::magazineEmpty;
				m_fireButton->setAutoRepeat(false);
				if (config.isAutoReloading())
					reloadAndPlay();
				break;
			}
		} else {
			m_state = WeaponState::magazineEmpty;
			m_noAmmoSound.play();
			m_fireButton->setAutoRepeat(false);
			if (config.isAutoReloading())
				reloadAndPlay();
			break;
		}

		if (m_semiAutomaticFireSwitch->state() && config.semiAutomaticAllowed)
			m_fireButton->setAutoRepeat(false);

		if (m_automaticFireSwitch->state() && config.automaticAllowed)
			m_fireButton->setAutoRepeat(true);
		break;

	case WeaponState::magazineRemoved:
	case WeaponState::otherMagazineInserted:
	case WeaponState::magazineEmpty:
		m_noAmmoSound.play();
		info << "Magazine is empty\n";
		break;

	case WeaponState::reloading:
	default:
		break;
	}
	trace << "State: " << m_state;
}

void Rifle::prepareAndSendShotMsg()
{
	RCSPStream stream(m_aggregator);
	ShotMessage msg;
	msg.damage = config.damageMin;
	msg.playerId = rifleOwner.playerId;
	msg.teamId = rifleOwner.teamId;
	stream.addCall(ConfigCodes::HeadSensor::Functions::catchShot, msg);
	m_irPresentationTransmitter->sendMessage(stream);
}

void Rifle::distortBolt(bool)
{
	switch(m_state)
	{
	case WeaponState::magazineEmpty:
		if (config.isReloadingByDistortingTheBolt())
		{
			reloadAndPlay();
		}
		break;
	case WeaponState::magazineReturned:
		if (!config.reloadNeedMagChange)
		{
			reloadAndPlay();
		}
		break;
	case WeaponState::otherMagazineInserted:
		reloadAndPlay();
		break;
	case WeaponState::ready:
		if (config.isReloadingByDistortingTheBolt())
		{
			reloadAndPlay();
		} else {
			/// @todo Add here removing of one shell
		}
		break;
	case WeaponState::reloading:
	default:
		break;

	}
	trace << "State: " << m_state;
}

void Rifle::magazineSensor(bool isConnected, uint8_t sensorNumber, bool isFirst)
{
	UNUSED_ARG(isFirst);
	info << "Sensor cb for  " << sensorNumber;
	if (!isConnected)
	{
		m_state = WeaponState::magazineRemoved;
		m_fireButton->setAutoRepeat(false);
	} else {
		switch(m_state)
		{

		case WeaponState::magazineRemoved:
			if (config.reloadNeedMagChange)
			{
				if (sensorNumber != m_currentMagazineNumber) // even if m_currentMagazineNumber == 0
				{
					// So NEW magazine was inserted
					m_state = WeaponState::otherMagazineInserted;
					if (!config.reloadNeedBolt)
					{
						m_currentMagazineNumber = sensorNumber;
						reloadAndPlay();
					}
				} else {
					// Old magazine was inserted
					m_state = WeaponState::magazineReturned;
					m_fireButton->setAutoRepeat(true);
				}
			} else {
				m_state = WeaponState::otherMagazineInserted;
				if (!config.reloadNeedBolt)
				{
					m_currentMagazineNumber = sensorNumber;
					reloadAndPlay();
				}
			}
			break;
		case WeaponState::magazineReturned:
		case WeaponState::magazineEmpty:
		case WeaponState::otherMagazineInserted:
		case WeaponState::ready:
		case WeaponState::reloading:
			warning << "Double magazine inserting...\n";
			break;
		default:
			break;
		}
	}
	info << "State: " << m_state;
}

uint8_t Rifle::getCurrentMagazineNumber()
{
	if (m_magazine1Sensor->state())
		return 1;
	else if (m_magazine2Sensor->state())
		return 2;
	else
		return 0;
}

void Rifle::reloadAndPlay()
{
	if (state.magazinesCountCurrent == 0)
	{
		info << "No more magazines!\n";
		m_noMagazines.play();
		return;
	}
	m_state = WeaponState::reloading;
	m_tasksPool.addOnce(
		[this]() {
			m_state = WeaponState::ready;
			m_fireButton->setAutoRepeat(true);
			state.magazinesCountCurrent--;
			state.bulletsInMagazineCurrent = config.bulletsInMagazineMax;
		},
		config.reloadingTime
	);
	info << "reloading\n";
	if (config.reloadPlaySound)
		m_reloadingSound.play();
}

bool Rifle::isSafeSwitchSelected()
{
	return !(m_automaticFireSwitch->state() || m_semiAutomaticFireSwitch->state());
}

/*
bool Rifle::isReloading()
{
	return (systemClock->getTime() - state.lastReloadTime < config.reloadingTime);
}*/

void Rifle::updatePlayerState()
{
	playerState.syncAll();
}

void Rifle::rifleTurnOff()
{
	info << "Rifle turned OFF";
	state.isEnabled = false;
	m_fireButton->turnOff();
	m_reloadButton->turnOff();
}

void Rifle::rifleTurnOn()
{
	info << "Rifle turned ON";
	state.isEnabled = true;
	m_fireButton->turnOn();
	m_reloadButton->turnOn();
	detectReloadAndMagsState();
}

void Rifle::rifleReset()
{
	state.reset();
	detectReloadAndMagsState();
	info << "Rifle resetted";
}

void Rifle::rifleRespawn()
{
	rifleReset();
	rifleTurnOn();
	updatePlayerState();
	m_respawnSound.play();
	info << "Rifle respawned";
}

void Rifle::rifleDie()
{
	info << "Rifle was told that player dead";
	if (!state.isEnabled)
		return;

	rifleTurnOff();
	m_dieSound.play();
}

void Rifle::headSensorToRifleHeartbeat(HSToRifleHeartbeat heartbeatMsg)
{
	m_lastHSHeartBeat = systemClock->getTime();

	if (heartbeatMsg.isEnabled() && !state.isEnabled)
		rifleTurnOn();
	else if (!heartbeatMsg.isEnabled() && state.isEnabled)
		rifleTurnOff();
	rifleOwner.teamId = heartbeatMsg.team;
	rifleOwner.playerId = heartbeatMsg.player;
	playerState.healthCurrent = heartbeatMsg.healthCurrent;
	playerState.healthMax = heartbeatMsg.healthMax;

	onHSConnected();
}

void Rifle::rifleWound()
{
	m_woundSound.play();
}

void Rifle::rifleRFIDProgramHSAddr()
{
	m_rfidProgrammingInProcess.play();
	state.rfidState = RifleState::RFIDSate::programHSAddr;
	m_rfidController.writeHSAddress(config.headSensorAddr);
}

void Rifle::rifleRFIDProgramServiceCard()
{
	m_rfidProgrammingInProcess.play();
	state.rfidState = RifleState::RFIDSate::programMasterCard;
	m_rfidController.writeServiceCard();
}

void Rifle::playDamagerNotification(uint8_t state)
{
	switch (state)
	{
	default:
	case NotificationSoundCase::enemyInjured:
		m_enemyDamaged.play();
		break;
	case NotificationSoundCase::enemyKilled:
		m_enemyKilled.play();
		break;
	case NotificationSoundCase::friendInjured:
		m_friendDamaged.play();
		break;
	}
}

void Rifle::checkHeartBeat()
{
	if (!isHSConnected())
	{
		// If heartbeat timeout
		info << "No heartbeat! Turning off";
		m_noHeartbeat.play();
		onHSDisconnected();
	} else {
		// If heartbeat is good
		onHSConnected();
	}
}

void Rifle::onHSConnected()
{
	if (!state.isHSConnected)
	{
		state.isHSConnected = true;
		info << "Head sensor connecter just now!";
		m_connectedToHeadSensorSound.play();
	}
}

void Rifle::onHSDisconnected()
{
	state.isHSConnected = false;
	if (state.isEnabled)
	{
		rifleTurnOff();
	}
	// Mark heartbeat time as outdated
	m_lastHSHeartBeat = systemClock->getTime() - 2 * maxNoHeartbeatDelay;
}

bool Rifle::isShocked()
{
	return systemClock->getTime() <= m_unshockTime;
}

bool Rifle::isHSConnected()
{
	return systemClock->getTime() - m_lastHSHeartBeat < maxNoHeartbeatDelay;
}

void Rifle::cardOperationDoneCallback(RifleRFIDController::Mode mode)
{
	debug << "Card operation done";
	m_RFIDCardReaded.play();
	switch(mode)
	{
	case RifleRFIDController::Mode::readCommandFromCard:
		break;
	case RifleRFIDController::Mode::nothing:
	case RifleRFIDController::Mode::writeHSAddressToCard:
	case RifleRFIDController::Mode::writeServiceCard:
		state.rfidState = RifleState::RFIDSate::reading;
		m_rfidController.readCommands();
		break;
	}
}

void Rifle::sendHeartbeatToHS()
{
	RCSPStream stream(m_aggregator);

	// @todo use RemoteCall
	stream.addCall(ConfigCodes::HeadSensor::Functions::rifleToHeadSensorHeartbeat);
	stream.send(
			m_networkClient,
			config.headSensorAddr,
			false,
			nullptr,
			std::forward<PackageTimings>(riflePackageTimings.heartbeat)
	);
}

void Rifle::riflePlayEnemyDamaged(uint8_t state)
{
	info << "Player damaged with state: " << state;
	playDamagerNotification(state);
}

void Rifle::rifleShock(uint32_t delay)
{
	if (delay == 0)
		return; // May be head sensor mistaked?

	m_unshockTime = systemClock->getTime() + delay;
}

void Rifle::rifleChangeHS(DeviceAddress newAddr)
{
	onHSDisconnected();
	config.headSensorAddr = newAddr;
}

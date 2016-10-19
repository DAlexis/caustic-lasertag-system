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


#include "head-sensor/resources.hpp"
#include "head-sensor/head-sensor.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "core/os-wrappers.hpp"
#include "core/logging.hpp"
#include "core/device-initializer.hpp"
#include "core/power-monitor.hpp"
#include "core/diagnostic.hpp"

#include "dev/nrf24l01.hpp"

#include "ir/ir-physical-tv.hpp"
#include "ir/ir-presentation-mt2.hpp"

#include <stdio.h>

#include <math.h>

WeaponManager::~WeaponManager()
{
	dropAllPackages();
}

void WeaponManager::assign(const DeviceAddress& addr)
{
	m_addr = addr;
}

void WeaponManager::dropAllPackages()
{
	auto checkAndDrop = [](PackageId& id) {
		if (id != 0)
		{
			NetworkLayer::instance().stopSending(id);
			id = 0;
		}
	};

	checkAndDrop(m_respawnPackage);
	checkAndDrop(m_diePackage);
}

void WeaponManager::respawn()
{
	if (m_respawnPackage != 0)
	{
		NetworkLayer::instance().updateTimeout(m_respawnPackage);
	} else {
		info << "WeaponManager::respawn() sending package";
		m_respawnPackage = RCSPStream::remoteCall
			(
				m_addr,
				ConfigCodes::Rifle::Functions::rifleRespawn,
				true,
				[this](PackageId, bool){ m_respawnPackage = 0; }
			);
	}
}

void WeaponManager::die()
{
	if (m_diePackage != 0)
	{
		NetworkLayer::instance().updateTimeout(m_diePackage);
	} else {
		m_diePackage = RCSPStream::remoteCall
			(
				m_addr,
				ConfigCodes::Rifle::Functions::rifleDie,
				true,
				[this](PackageId, bool){ m_diePackage = 0; },
				std::move(headSensorPackageTimings.killPlayer)
			);
	}
}

IWeaponObresver *WeaponManagerFactory::create() const
{
	return new WeaponManager();
}

HeadSensor::HeadSensor()
{
	m_tasksPool.setStackSize(256);
	deviceConfig.deviceType = DeviceTypes::headSensor;
	playerState.weaponsList.setWeaponObserverFactory(&weaponManagerFactory);
	//m_weapons.insert({1,1,1});
}

void HeadSensor::init(const Pinout &_pinout)
{
	//debug.enable();

	info << "Configuring power monitor";
	PowerMonitor::instance().interrogate();

	info << "Configuring kill zones";

	m_irPresentationReceiversGroup = new PresentationReceiversGroupMT2;
	m_killZonesInterogator.setStackSize(512);
	m_killZonesInterogator.setName("KZintrg");

	m_killZonesInterogator.registerObject(m_irPresentationReceiversGroup);

	auto initKillZone = [this](uint8_t index, IIOPin* pin, IIOPin* vibroPin, FloatParameter* damageCoefficient) {
		m_irPhysicalReceivers[index] = new IRReceiverTV;
		m_irPhysicalReceivers[index]->setIOPin(pin);
		m_irPhysicalReceivers[index]->init();
		m_irPhysicalReceivers[index]->setEnabled(true);

		m_irPresentationReceivers[index] = new IRPresentationReceiverMT2;
		m_irPresentationReceivers[index]->setPhysicalReceiver(m_irPhysicalReceivers[index]);
		m_irPresentationReceivers[index]->setDamageCoefficient(damageCoefficient);
		m_irPresentationReceivers[index]->setVibroEngine(vibroPin);
		m_irPresentationReceivers[index]->init();

		m_irPresentationReceiversGroup->connectReceiver(*(m_irPresentationReceivers[index]));
	};

	for (int i=0; i<6; i++)
	{
		char zoneName[10];
		char vibroName[20];
		sprintf(zoneName, "zone%d", i+1);
		sprintf(vibroName, "zone%d_vibro", i+1);
		const Pinout::PinDescr& zone = _pinout[zoneName];
		const Pinout::PinDescr& zoneVibro = _pinout[vibroName];
		if (zone)
		{
			initKillZone(i,
					IOPins->getIOPin(zone.port, zone.pin),
					IOPins->getIOPin(zoneVibro.port, zoneVibro.pin),
					&playerConfig.zone1DamageCoeff
				);
		}
	}

	info << "Parsing config file";

	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values";
		playerConfig.setDefault();
	}

	info << "Restoring last state and config";
	MainStateSaver::instance().setFilename("state-save");
	// State restoring is always after config reading, so not stored data will be default
	if (MainStateSaver::instance().tryRestore())
	{
		info << "  restored";
	} else {
		error << "  restoring failed, using default config";
		// setting player state to default
		playerState.reset();
		MainStateSaver::instance().saveState();
	}

	info << "Initializing visual effects";
	/// @todo Add support for only red (and LED-less) devices
	m_leds.init(
			IOPins->getIOPin(_pinout["red"].port, _pinout["red"].pin),
			IOPins->getIOPin(_pinout["green"].port, _pinout["green"].pin),
			IOPins->getIOPin(_pinout["blue"].port, _pinout["blue"].pin)
			);
	m_leds.blink(blinkPatterns.init);

	info << "Network initialization";
	NetworkLayer::instance().setAddress(deviceConfig.devAddr);
	NetworkLayer::instance().setPackageReceiver(RCSPMultiStream::getPackageReceiver());
	NetworkLayer::instance().registerBroadcast(broadcast.any);
	NetworkLayer::instance().registerBroadcast(broadcast.headSensors);
	NetworkLayer::instance().registerBroadcastTester(new TeamBroadcastTester(playerConfig.teamId));

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



#ifdef DEBUG
	NetworkLayer::instance().enableDebug(true);
#endif

	//NetworkLayer::instance().enableRegularNRFReinit(true);

	info << "Other initialization";
	m_tasksPool.add(
			[this] { m_taskPoolStager.stage("sendHeartbeat()"); sendHeartbeat(); },
			heartbeatPeriod
	);

	m_tasksPool.add(
			[this] { m_taskPoolStager.stage("m_statsCounter.interrogate()"); m_statsCounter.interrogate(); },
			200000
	);

	m_tasksPool.add(
			[this] { m_taskPoolStager.stage("PowerMonitor::interrogate()"); PowerMonitor::instance().interrogate(); },
			100000
	);

	m_tasksPool.add(
			[this] { m_mfrcWrapper.interrogate(); },
			10000
	);

	info << "Stats restoring";
	m_statsCounter.restoreFromFile();

	MainStateSaver::instance().registerStateSaver(&m_statsCounter);

	m_tasksPool.run();
	MainStateSaver::instance().runSaver(8000);

	m_killZonesInterogator.run();
	info << "Head sensor ready to use";


	m_mfrcWrapper.init();
	setFRIDToWriteAddr();

	// Statistics dubug
	m_statsCounter.clear();
	m_statsCounter.registerHit(25);
	m_statsCounter.registerHit(26);
	m_statsCounter.registerHit(27);
	m_statsCounter.registerDamage(25, 8);
	m_statsCounter.registerDamage(26, 9);
	m_statsCounter.registerDamage(27, 10);
	m_statsCounter.registerKill(25);
	m_statsCounter.registerKill(27);
}

void HeadSensor::resetToDefaults()
{
	m_callbackStager.stage("reset");
	info << "Resetting configuration to default";
	m_leds.blink(blinkPatterns.anyCommand);
	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values";
		playerConfig.setDefault();
	}
	playerState.reset();
	MainStateSaver::instance().resetSaves();
	MainStateSaver::instance().saveState();
}


void HeadSensor::catchShot(ShotMessage msg)
{
	m_callbackStager.stage("shot");
	info << "** Shot - team: " << msg.teamId << ", player: " << msg.playerId << ", damage: " << msg.damage;
	Time currentTime = systemClock->getTime();
	if (currentTime - m_shockDelayBegin < playerConfig.shockDelayImmortal)
	{
		info << "!! Shock time";
	}
	else if (playerState.isAlive()) {

		if (msg.playerId == playerConfig.plyerMT2Id)
		{
			debug << "self-shot";
			msg.damage *= playerConfig.selfShotCoeff;
		}
		else if (msg.teamId == playerConfig.teamId)
		{
			debug << "friendly fire";
			msg.damage *= playerConfig.frendlyFireCoeff;
		}

		UintParameter healthBeforeDamage = playerState.healthCurrent;

		debug << "total damage: " << msg.damage;
		playerState.damage(msg.damage);

		info << "health: " <<  playerState.healthCurrent << " armor: " << playerState.armorCurrent;

		// If still is alive
		if (playerState.isAlive())
		{
			if (msg.damage != 0)
			{
				m_shockDelayBegin = systemClock->getTime();
				weaponWoundAndShock();
				m_statsCounter.registerHit(msg.playerId);
				m_statsCounter.registerDamage(msg.playerId, msg.damage);
			}
			m_leds.blink(blinkPatterns.wound);
			if (msg.playerId != playerConfig.plyerMT2Id)
			{
				notifyDamager(msg.playerId, msg.teamId, DamageNotification::injured);
			}
		} else {
			//Player was killed
			info << "xx Player died";
			dieWeapons();
			if (msg.playerId != playerConfig.plyerMT2Id)
			{
				notifyDamager(msg.playerId, msg.teamId, DamageNotification::killed);
			}
			m_statsCounter.registerKill(msg.playerId);
			m_statsCounter.registerDamage(msg.playerId, healthBeforeDamage);
			m_leds.blink(blinkPatterns.death);
			/// @todo reenable
			//Scheduler::instance().addTask(std::bind(&StateSaver::saveState, &StateSaver::instance()), true, 0, 0, 1000000);
		}
	}
}

void HeadSensor::playerRespawn()
{
	m_callbackStager.stage("respawn");
	if (!playerState.respawn())
	{
		info << "Respawn limit is over!";
		// @todo Add any notification that respawn limit is over
		m_leds.blink(blinkPatterns.respawnLimitIsOver);
		return;
	}
	m_leds.blink(blinkPatterns.respawn);
	respawnWeapons();
	info << "Player spawned";
/*
	std::function<void(void)> respawnFunction = [this] {
		playerState.respawn();
		respawnWeapons();
		printf("Player spawned");
	};
	Scheduler::instance().addTask(respawnFunction, true, 0, 0, systemClock->getTime() + playerConfig.postRespawnDelay);
*/
}

void HeadSensor::playerReset()
{
	playerState.reset();
	respawnWeapons();
	info << "Player reseted";
}

void HeadSensor::playerKill()
{
	if (!playerState.isAlive())
		return;
	playerState.kill();
	m_leds.blink(blinkPatterns.death);
	dieWeapons();
	info << "Player killed with kill command";
}

void HeadSensor::resetStats()
{
	m_statsCounter.clear();
}

void HeadSensor::readStats()
{
	debug << "Stats reading requested";
	m_statsCounter.sendStats();
}

void HeadSensor::dieWeapons()
{
	m_callbackStager.stage("die weapons");
	/// Notifying weapons
	for (auto it = playerState.weaponsList.weapons().begin(); it != playerState.weaponsList.weapons().end(); it++)
	{
		info << "Sending kill signal to weapon...";
		static_cast<WeaponManager*>(it->second)->die();
		//RCSPStream::remoteCall(it->first, ConfigCodes::Rifle::Functions::rifleDie, true, nullptr, std::move(headSensorPackageTimings.killPlayer));
	}
}

void HeadSensor::respawnWeapons()
{
	m_callbackStager.stage("respawn weapons");
	/// Notifying weapons
	for (auto it = playerState.weaponsList.weapons().begin(); it != playerState.weaponsList.weapons().end(); it++)
	{
		info << "Resetting weapon...";
		static_cast<WeaponManager*>(it->second)->respawn();
		//RCSPStream::remoteCall(it->first, ConfigCodes::Rifle::Functions::rifleRespawn);
	}
}

void HeadSensor::turnOffWeapons()
{
	for (auto it = playerState.weaponsList.weapons().begin(); it != playerState.weaponsList.weapons().end(); it++)
	{
		info << "Turning off weapon...";
		RCSPStream::remoteCall(it->first, ConfigCodes::Rifle::Functions::rifleTurnOff);
	}
}

void HeadSensor::weaponWoundAndShock()
{
	m_callbackStager.stage("weaponWoundAndShock");
	info << "Weapons shock delay notification";
	for (auto it = playerState.weaponsList.weapons().begin(); it != playerState.weaponsList.weapons().end(); it++)
	{
		RCSPMultiStream stream;
		// We have 23 bytes free in one stream (and should try to use only one)
		stream.addCall(ConfigCodes::Rifle::Functions::rifleShock, playerConfig.shockDelayInactive); // 3b + 4b (16 free)
		stream.addCall(ConfigCodes::Rifle::Functions::rifleWound); // 3b (13 free)
		stream.addValue(ConfigCodes::HeadSensor::State::healthCurrent); // 3b + 2b (8 free)
		stream.addValue(ConfigCodes::HeadSensor::State::armorCurrent); // 3b + 2b (3 free)
		stream.send(it->first, true, std::move(headSensorPackageTimings.woundPlayer));
	}
}

void HeadSensor::sendHeartbeat()
{
	trace << "Sending heartbeat";
	for (auto it = playerState.weaponsList.weapons().begin(); it != playerState.weaponsList.weapons().end(); it++)
	{
		RCSPStream stream;
		// This line is temporary solution if team was changed by value, not by setter func call
		stream.addValue(ConfigCodes::HeadSensor::Configuration::teamId);
		stream.addValue(ConfigCodes::HeadSensor::State::healthCurrent);
		stream.addCall(ConfigCodes::Rifle::Functions::headSensorToRifleHeartbeat);
		stream.send(it->first, false);
		//RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::headSensorToRifleHeartbeat, false, nullptr);
	}
}

void HeadSensor::registerWeapon(DeviceAddress weaponAddress)
{
	info << "Registering weapon " << ADDRESS_TO_STREAM(weaponAddress);
	auto it = playerState.weaponsList.weapons().find(weaponAddress);
	if (it == playerState.weaponsList.weapons().end())
	{
		playerState.weaponsList.insert(weaponAddress);
	}

	RCSPStream stream;
	stream.addValue(ConfigCodes::HeadSensor::Configuration::plyerMT2Id);
	stream.addValue(ConfigCodes::HeadSensor::Configuration::teamId);
	stream.addCall(ConfigCodes::Rifle::Functions::headSensorToRifleHeartbeat);

	if (playerState.isAlive())
	{
		stream.addCall(ConfigCodes::Rifle::Functions::rifleTurnOn);
	} else {
		stream.addCall(ConfigCodes::Rifle::Functions::rifleTurnOff);
	}
	stream.send(weaponAddress, true);
}

void HeadSensor::deregisterWeapon(DeviceAddress weaponAddress)
{
	info << "Deregistering weapon " << ADDRESS_TO_STREAM(weaponAddress);
	auto it = playerState.weaponsList.weapons().find(weaponAddress);
	if (it != playerState.weaponsList.weapons().end())
		playerState.weaponsList.remove(weaponAddress);
	NetworkLayer::instance().dropAllForAddress(weaponAddress);
}

void HeadSensor::setTeam(uint8_t teamId)
{
	info << "Setting team id";
	playerConfig.teamId = teamId;
	m_leds.blink(blinkPatterns.anyCommand);
	for (auto it = playerState.weaponsList.weapons().begin(); it != playerState.weaponsList.weapons().end(); it++)
	{
		info << "Changing weapon team id to" << teamId;
		RCSPStream::remotePullValue(it->first, ConfigCodes::HeadSensor::Configuration::teamId);
	}
}

void HeadSensor::addMaxHealth(int16_t delta)
{
	info << "Adding health: " << delta;
	if (delta < 0 && playerConfig.healthMax < -delta)
	{
		debug << "Max health is " << playerConfig.healthMax << ", so can not add " << delta;
		return;
	}
	playerConfig.healthMax += delta;
	if (delta < 0 && playerConfig.healthStart < -delta)
	{
		debug << "Start health is " << playerConfig.healthStart << ", so can not add " << delta;
		return;
	}
	playerConfig.healthStart += delta;
	m_leds.blink(blinkPatterns.anyCommand);
}

void HeadSensor::notifyDamager(PlayerGameId damager, uint8_t damagerTeam, uint8_t state)
{
	UNUSED_ARG(damagerTeam);
	DamageNotification notification;
	notification.damager = damager;
	notification.damagedTeam = playerConfig.teamId;
	notification.state = state;
	notification.target = playerConfig.plyerMT2Id;
	info << "Notifying damager";
	RCSPStream::remoteCall(
			broadcast.headSensors,
			ConfigCodes::HeadSensor::Functions::notifyIsDamager,
			notification,
			true,
			nullptr,
			std::move(headSensorPackageTimings.damagerNotificationBroadcast)
			);
}

void HeadSensor::notifyIsDamager(DamageNotification notification)
{
	info << "By the time " << notification.damager << " damaged " << notification.target;
	if (notification.damager != playerConfig.plyerMT2Id)
		return;

	if (!playerState.weaponsList.weapons().empty())
	{
		uint8_t sound =
			(notification.damagedTeam == playerConfig.teamId) ? // testing for friendly fire
				NotificationSoundCase::friendInjured :
				notification.state;
		// Now notifying over first attached weapon
		RCSPStream::remoteCall(playerState.weaponsList.weapons().begin()->first, ConfigCodes::Rifle::Functions::riflePlayEnemyDamaged, sound);
	}

}

void HeadSensor::setFRIDToWriteAddr()
{
	uint16_t size = sizeof(m_RFIDWriteBuffer[0])*RFIDWriteBufferSize;
	memset(m_RFIDWriteBuffer, 0, size);
	uint16_t actualSize = 0;
	RCSPAggregator::instance().serializeCallRequest(
			m_RFIDWriteBuffer,
			ConfigCodes::Rifle::Functions::rifleChangeHS,
			size,
			actualSize,
			deviceConfig.devAddr
		);
	m_mfrcWrapper.writeBlock(
			m_RFIDWriteBuffer,
			RFIDWriteBufferSize,
			[this](uint8_t* data, uint16_t size)
			{
				UNUSED_ARG(data); UNUSED_ARG(size);
				m_leds.blink(blinkPatterns.anyCommand);
			}
		);
}

void HeadSensor::setDafaultPinout(Pinout& pinout)
{
	pinout.set("zone1", 0, 0);
	pinout.set("zone1Vibro", 2, 0);
	pinout.set("zone2", 0, 1);
	pinout.set("zone2Vibro", 2, 1);
	pinout.set("zone3", 0, 2);
	pinout.set("zone3Vibro", 2, 2);
	pinout.set("zone4", 0, 3);
	pinout.set("zone4Vibro", 2, 3);
	pinout.set("zone5", 0, 4);
	pinout.set("zone5Vibro", 2, 4);
	pinout.set("zone6", 0, 5);
	pinout.set("zone6Vibro", 2, 5);
	pinout.set("red", 1, 0);
	pinout.set("green", 0, 7);
	pinout.set("blue", 0, 6);
}

bool HeadSensor::checkPinout(const Pinout& pinout)
{
	/// @todo Add cheching here
	return true;
}


/////////////////////
// HeadSensor::TeamBroadcastTester
bool HeadSensor::TeamBroadcastTester::isAcceptableBroadcast(const DeviceAddress& addr)
{
	if (addr == broadcast.headSensorsRed && *m_pId == 0)
		return true;
	if (addr == broadcast.headSensorsBlue && *m_pId == 1)
		return true;
	if (addr == broadcast.headSensorsYellow && *m_pId == 2)
		return true;
	if (addr == broadcast.headSensorsGreen && *m_pId == 3)
		return true;
	return false;
}


/////////////////////
// Test functions
void HeadSensor::testDie(const char*)
{
	playerState.healthCurrent = 0;
	catchShot(ShotMessage());
}



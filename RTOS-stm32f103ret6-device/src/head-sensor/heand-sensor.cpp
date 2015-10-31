/*
 * head-sensor.cpp
 *
 *  Created on: 24 янв. 2015 г.
 *      Author: alexey
 */

#include "head-sensor/resources.hpp"
#include "head-sensor/head-sensor.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "core/os-wrappers.hpp"
#include "core/logging.hpp"
#include "core/device-initializer.hpp"

#include <stdio.h>

HeadSensor::HeadSensor()
{
	m_tasksPool.setStackSize(256);
	deviceConfig.deviceType = DeviceTypes::headSensor;
	//m_weapons.insert({1,1,1});
}

void HeadSensor::init(const Pinout &pinout)
{
	configure(pinout);
}

void HeadSensor::configure(const Pinout &_pinout)
{
	ScopedTag tag("head-init");

	//debug.enable();

	info << "Configuring kill zones";

	const Pinout::PinDescr& zone1 = _pinout["zone1"];
	const Pinout::PinDescr& zone1vibro = _pinout["zone1_vibro"];
	IIOPin* zone1vibroPin = zone1vibro ? IOPins->getIOPin(zone1vibro.port, zone1vibro.pin) : nullptr;
	if (zone1)
		m_killZonesManager.enableKillZone(
				0,
				IOPins->getIOPin(zone1.port, zone1.pin),
				zone1vibroPin
				);

	const Pinout::PinDescr& zone2 = _pinout["zone2"];
	const Pinout::PinDescr& zone2vibro = _pinout["zone2_vibro"];
	if (zone2)
		m_killZonesManager.enableKillZone(
				1,
				IOPins->getIOPin(zone2.port, zone2.pin),
				zone2vibro ? IOPins->getIOPin(zone2vibro.port, zone2vibro.pin) : zone1vibroPin
				);

	const Pinout::PinDescr& zone3 = _pinout["zone3"];
	const Pinout::PinDescr& zone3vibro = _pinout["zone3_vibro"];
	if (zone3)
		m_killZonesManager.enableKillZone(
				2,
				IOPins->getIOPin(zone3.port, zone3.pin),
				zone3vibro ? IOPins->getIOPin(zone3vibro.port, zone3vibro.pin) : zone1vibroPin
				);

	const Pinout::PinDescr& zone4 = _pinout["zone4"];
	const Pinout::PinDescr& zone4vibro = _pinout["zone4_vibro"];
	if (zone4)
		m_killZonesManager.enableKillZone(
				3,
				IOPins->getIOPin(zone4.port, zone4.pin),
				zone4vibro ? IOPins->getIOPin(zone4vibro.port, zone4vibro.pin) : zone1vibroPin
				);

	const Pinout::PinDescr& zone5 = _pinout["zone5"];
	const Pinout::PinDescr& zone5vibro = _pinout["zone5_vibro"];
	if (zone5)
		m_killZonesManager.enableKillZone(
				4,
				IOPins->getIOPin(zone5.port, zone5.pin),
				zone5vibro ? IOPins->getIOPin(zone5vibro.port, zone5vibro.pin) : zone1vibroPin
				);

	const Pinout::PinDescr& zone6 = _pinout["zone6"];
	const Pinout::PinDescr& zone6vibro = _pinout["zone6_vibro"];
	if (zone6)
		m_killZonesManager.enableKillZone(
				5,
				IOPins->getIOPin(zone6.port, zone6.pin),
				zone6vibro ? IOPins->getIOPin(zone6vibro.port, zone6vibro.pin) : zone1vibroPin
				);

	//m_mainSensor.enableDebug(true);


	info << "Parsing config file";

	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values";
		playerConfig.setDefault();
	}


	info << "Restoring last state and config";
	StateSaver::instance().setFilename("state-save");
	// State restoring is always after config reading, so not stored data will be default
	if (StateSaver::instance().tryRestore())
	{
		info << "  restored";
	} else {
		error << "  restoring failed, using default config";
		// setting player state to default
		playerState.reset();
		StateSaver::instance().saveState();
	}

	info << "Initializing visual effects";
	/// @todo Add support for only red (and LED-less) devices
	m_leds.init(
			IOPins->getIOPin(_pinout["red"].port, _pinout["red"].pin),
			IOPins->getIOPin(_pinout["green"].port, _pinout["green"].pin),
			IOPins->getIOPin(_pinout["blue"].port, _pinout["blue"].pin)
			);
	m_leds.blink(blinkPatterns.init);

	info << "Other initialization";
	NetworkLayer::instance().setAddress(deviceConfig.devAddr);
	NetworkLayer::instance().setPackageReceiver(RCSPMultiStream::getPackageReceiver());
	NetworkLayer::instance().registerBroadcast(broadcast.any);
	NetworkLayer::instance().registerBroadcast(broadcast.headSensors);
	NetworkLayer::instance().registerBroadcastTester(new TeamBroadcastTester(playerConfig.teamId));
	NetworkLayer::instance().init();

	m_tasksPool.add(
			[this]() { sendHeartbeat(); },
			heartbeatPeriod
	);

	m_tasksPool.run();
	StateSaver::instance().runSaver(8000);

	info << "Head sensor ready to use";
}

void HeadSensor::resetToDefaults()
{
	//ScopedTag tag("reset");
	info << "Resetting configuration to default";
	m_leds.blink(blinkPatterns.anyCommand);
	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		error << "Cannot read config file, so setting default values";
		playerConfig.setDefault();
	}
	playerState.reset();
	StateSaver::instance().resetSaves();
	StateSaver::instance().saveState();
}


void HeadSensor::catchShot(ShotMessage msg)
{
	//ScopedTag tag("shot-cb");
	info << "** Shot - team: " << msg.teamId << ", player: " << msg.playerId << ", damage: " << msg.damage;
	Time currentTime = systemClock->getTime();
	if (currentTime - m_shockDelayBegin < playerConfig.shockDelayImmortal)
	{
		info << "!! Shock time";
	}
	else if (playerState.isAlive()) {

		if (msg.playerId == playerConfig.plyerMT2Id)
		{
			msg.damage *= playerConfig.selfShotCoeff;
		}
		else if (msg.teamId == playerConfig.teamId)
		{
			msg.damage *= playerConfig.frendlyFireCoeff;
		}

		playerState.damage(msg.damage);

		info << "health: " <<  playerState.healthCurrent << " armor: " << playerState.armorCurrent;

		// If still is alive
		if (playerState.isAlive())
		{
			if (msg.damage != 0)
			{
				m_shockDelayBegin = systemClock->getTime();
				weaponWoundAndShock();
			}
			m_leds.blink(blinkPatterns.wound);
			notifyDamager(msg.playerId, msg.teamId, DamageNotification::injured);
		} else {
			//Player was killed
			info << "xx Player died";
			dieWeapons();
			notifyDamager(msg.playerId, msg.teamId, DamageNotification::killed);
			m_leds.blink(blinkPatterns.death);
			/// @todo reenable
			//Scheduler::instance().addTask(std::bind(&StateSaver::saveState, &StateSaver::instance()), true, 0, 0, 1000000);
		}
	}
}

void HeadSensor::playerRespawn()
{
	ScopedTag tag("respawn");
	playerState.respawn();
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
	catchShot(ShotMessage {0, 0, 0});
	dieWeapons();
	info << "Player killed";
}

void HeadSensor::dieWeapons()
{
	ScopedTag tag("die-weapons");
	/// Notifying weapons
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		info << "Sending kill signal to weapon...";
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleDie, true, nullptr, std::move(headSensorPackageTimings.killPlayer));
	}
}

void HeadSensor::respawnWeapons()
{
	/// Notifying weapons
	ScopedTag tag("reset-weapons");
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		info << "Resetting weapon...";
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleRespawn);
	}
}

void HeadSensor::turnOffWeapons()
{
	ScopedTag tag("turn-off");
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		info << "Turning off weapon...";
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleTurnOff);
	}
}

void HeadSensor::weaponWoundAndShock()
{
	info << "Weapons shock delay notification";
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		RCSPMultiStream stream;
		// We have 23 bytes free in one stream (and should try to use only one)
		stream.addCall(ConfigCodes::Rifle::Functions::rifleShock, playerConfig.shockDelayInactive); // 3b + 4b (16 free)
		stream.addCall(ConfigCodes::Rifle::Functions::rifleWound); // 3b (13 free)
		stream.addValue(ConfigCodes::HeadSensor::State::healthCurrent); // 3b + 2b (8 free)
		stream.addValue(ConfigCodes::HeadSensor::State::armorCurrent); // 3b + 2b (3 free)
		stream.send(*it, true, std::move(headSensorPackageTimings.woundPlayer));
	}
}

void HeadSensor::sendHeartbeat()
{
	if (!playerState.isAlive())
		return;
	trace << "Sending heartbeat";
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		RCSPStream stream;
		// This line is temporary solution if team was changed by value, not by setter func call
		stream.addValue(ConfigCodes::HeadSensor::Configuration::teamId);
		stream.addValue(ConfigCodes::HeadSensor::State::healthCurrent);
		stream.addCall(ConfigCodes::Rifle::Functions::headSensorToRifleHeartbeat);
		stream.send(*it, false);
		//RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::headSensorToRifleHeartbeat, false, nullptr);
	}
}

void HeadSensor::registerWeapon(DeviceAddress weaponAddress)
{
	info << "Registering weapon " << ADDRESS_TO_STREAM(weaponAddress);
	auto it = playerState.weaponsList.weapons.find(weaponAddress);
	if (it == playerState.weaponsList.weapons.end())
	{
		playerState.weaponsList.weapons.insert(weaponAddress);
	}

	RCSPStream stream;
	stream.addValue(ConfigCodes::HeadSensor::Configuration::plyerMT2Id);
	stream.addValue(ConfigCodes::HeadSensor::Configuration::teamId);

	if (playerState.isAlive())
	{
		stream.addCall(ConfigCodes::Rifle::Functions::rifleTurnOn);
	} else {
		stream.addCall(ConfigCodes::Rifle::Functions::rifleTurnOff);
	}
	stream.send(weaponAddress, true);
}

void HeadSensor::setTeam(uint8_t teamId)
{
	info << "Setting team id";
	playerConfig.teamId = teamId;
	m_leds.blink(blinkPatterns.anyCommand);
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		info << "Changing weapon team id to" << teamId;
		RCSPStream::remotePullValue(*it, ConfigCodes::HeadSensor::Configuration::teamId);
	}
}

void HeadSensor::addMaxHealth(int16_t delta)
{
	ScopedTag tag("set-team");
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

void HeadSensor::notifyDamager(PlayerMT2Id damager, uint8_t damagerTeam, uint8_t state)
{
	UNUSED_ARG(damagerTeam);
	ScopedTag tag("notify-damager");
	DamageNotification notification;
	notification.damager = damager;
	notification.damagedTeam = playerConfig.teamId;
	notification.state = state;
	notification.target = playerConfig.plyerMT2Id;
	info << "Notifying damager";
	RCSPStream::remoteCall(broadcast.headSensors, ConfigCodes::HeadSensor::Functions::notifyIsDamager, notification, false);
}

void HeadSensor::notifyIsDamager(DamageNotification notification)
{
	ScopedTag tag("notify-damaged");
	info << "By the time " << notification.damager << " damaged " << notification.target;
	if (notification.damager != playerConfig.plyerMT2Id)
		return;

	if (!playerState.weaponsList.weapons.empty())
	{
		uint8_t sound =
			notification.damagedTeam == playerConfig.teamId ?
				NotificationSoundCase::friendInjured :
				notification.state;
		RCSPStream::remoteCall(*(playerState.weaponsList.weapons.begin()), ConfigCodes::Rifle::Functions::riflePlayEnemyDamaged, sound);
	}

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
	catchShot(ShotMessage {0, 0, 0});
}



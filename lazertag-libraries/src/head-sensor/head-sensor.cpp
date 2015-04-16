/*
 * head-sensor.cpp
 *
 *  Created on: 24 янв. 2015 г.
 *      Author: alexey
 */

#include "head-sensor/head-sensor.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "dev/console.hpp"
#include "core/scheduler.hpp"
#include "hal/ext-interrupts.hpp"
#include "dev/sdcard-fs.hpp"

#include <stdio.h>

HeadSensor::HeadSensor()
{
	//m_weapons.insert({1,1,1});
}

void HeadSensor::configure()
{
	IExternalInterruptManager *exti = EXTIS->getEXTI(0);
	exti->init(0);
	exti->turnOn();

	m_mainSensor.setShortMessageCallback(std::bind(&HeadSensor::shotCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	m_mainSensor.init(exti);
	//m_mainSensor.enableDebug(true);

	printf("- Mounting sd-card\n");
	if (!SDCardFS::instance().init())
		printf("Error during mounting sd-card!\n");

	RCSPModem::instance().init();

	printf("- Creating console commands for head sensor\n");
	Console::instance().registerCommand(
		"die",
		"kill player",
		std::bind(&HeadSensor::testDie, this, std::placeholders::_1)
	);

	printf("- Parsing config file\n");
	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		printf("Cannot read config file, so setting default values");
		playerConfig.setDefault();
	}

	StateSaver::instance().setFilename("state-save");
	// State restoring is always after config reading, so not stored data will be default
	printf("- Restoring state\n");
	if (StateSaver::instance().tryRestore())
	{
		printf("  restored\n");
	} else {
		printf("  restored failed\n");
		// setting player state to default
		playerState.reset();
	}

	printf("- Initializing visual effects");
	m_leds.init(IOPins->getIOPin(1, 0), IOPins->getIOPin(0, 7), IOPins->getIOPin(0, 6));
	m_leds.setColor(getTeamColor());
	m_leds.blink(50000, 150000, 5);

	StateSaver::instance().runSaver(10000000);

	printf("- Other initialization\n");
	RCSPModem::instance().registerBroadcast(broadcast.any);
	RCSPModem::instance().registerBroadcast(broadcast.headSensors);
	printf("Head sensor ready to use\n");
}

void HeadSensor::resetToDefaults()
{
	printf("Resetting player configuration\n");
	if (!RCSPAggregator::instance().readIni("config.ini"))
	{
		printf("Cannot read config file, so setting default values");
		playerConfig.setDefault();
	}
	playerState.reset();
}


void HeadSensor::shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage)
{
	printf("Cought a shot: team %d, player %d, damage: %d\n", teamId, playerId, damage);
	if (playerState.isAlive()) {

		if (playerId == playerConfig.plyerMT2Id)
		{
			damage *= playerConfig.selfShotCoeff;
		}
		else if (teamId == playerConfig.teamId)
		{
			damage *= playerConfig.frendlyFireCoeff;
		}

		playerState.damage(damage);
		printf("health: %u, armor: %u\n", playerState.healthCurrent, playerState.armorCurrent);
		for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
		{
			RCSPStream stream;
			stream.addValue(ConfigCodes::HeadSensor::State::healthCurrent);
			stream.addValue(ConfigCodes::HeadSensor::State::armorCurrent);
			stream.send(*it, true, nullptr);
		}
		if (!playerState.isAlive())
			dieWeapons();
	}
	if (!playerState.isAlive()) {
		printf("Player died\n");
		m_leds.blink(100000, 100000, 100);
		/// Notifying weapons
		turnOffWeapons();
	} else {
		m_leds.blink(100000, 100000, 1);
	}
	StateSaver::instance().saveState();
}

void HeadSensor::playerRespawn()
{
	playerState.respawn();
	m_leds.blink(100000, 100000, 2);
	respawnWeapons();
	printf("Player spawned\n");
/*
	std::function<void(void)> respawnFunction = [this] {
		playerState.respawn();
		respawnWeapons();
		printf("Player spawned\n");
	};
	Scheduler::instance().addTask(respawnFunction, true, 0, 0, systemClock->getTime() + playerConfig.postRespawnDelay);
*/
}

void HeadSensor::playerReset()
{
	playerState.reset();
	respawnWeapons();
	printf("Player reseted\n");
}

void HeadSensor::playerKill()
{
	printf("Player killed\n");
	if (!playerState.isAlive())
		return;
	playerState.kill();
	shotCallback(0, 0, 0);
	//dieWeapons();
}

void HeadSensor::dieWeapons()
{
	/// Notifying weapons
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		printf("Sending kill signal...\n");
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleDie);
	}
}

void HeadSensor::respawnWeapons()
{
	/// Notifying weapons
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		printf("Resetting weapon...\n");
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleRespawn);
	}
}

void HeadSensor::turnOffWeapons()
{
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		printf("Turning off weapon\n");
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleTurnOff);
	}
}

void HeadSensor::registerWeapon(DeviceAddress weaponAddress)
{
	printf("Registering weapon\n");
	weaponAddress.print();
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
	printf("Setting team id\n");
	playerConfig.teamId = teamId;
	m_leds.setColor(getTeamColor());
	m_leds.blink(100000, 100000, 2);
	for (auto it = playerState.weaponsList.weapons.begin(); it != playerState.weaponsList.weapons.end(); it++)
	{
		printf("Changing weapon team id to %u\n", teamId);
		RCSPStream::remotePullValue(*it, ConfigCodes::HeadSensor::Configuration::teamId);
	}

}

uint8_t HeadSensor::getTeamColor()
{
	switch (playerConfig.teamId)
	{
	case 0: return RGBLeds::red;
	case 1: return RGBLeds::blue;
	case 2: return RGBLeds::red | RGBLeds::green;
	case 3: return RGBLeds::green;
	default: return RGBLeds::red | RGBLeds::green | RGBLeds::blue;
	}
}


/////////////////////
// Test functions
void HeadSensor::testDie(const char*)
{
	playerState.healthCurrent = 0;
	shotCallback(0, 0, 0);
}

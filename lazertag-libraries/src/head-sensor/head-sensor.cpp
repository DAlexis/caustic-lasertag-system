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
	RCSPAggregator::instance().readIni("config.ini");

	playerReset();
	printf("Head sensor ready to use\n");
}

void HeadSensor::shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage)
{
	printf("Cought a shot: team %d, player %d, damage: %d\n", teamId, playerId, damage);
	if (playerState.isAlive()) {
		playerState.damage(damage);
		printf("health: %u, armor: %u\n", playerState.healthCurrent, playerState.armorCurrent);
		for (auto it = m_weapons.begin(); it != m_weapons.end(); it++)
		{
			RCSPStream stream;
			stream.addValue(ConfigCodes::HeadSensor::State::healthCurrent);
			stream.addValue(ConfigCodes::HeadSensor::State::armorCurrent);
			stream.send(*it, true, nullptr);
		}
	}
	if (!playerState.isAlive()) {
		printf("Player died\n");
		/// Notifying weapons
		turnOffWeapons();
	}
}

void HeadSensor::playerRespawn()
{
	playerState.respawn();
	turnOnAndResetWeapons();
	printf("Player spawned\n");
}

void HeadSensor::playerReset()
{
	playerState.reset();
	turnOnAndResetWeapons();
	printf("Player reseted\n");
}

void HeadSensor::playerKill()
{
	printf("Player killed\n");
	playerState.kill();
}

void HeadSensor::turnOnAndResetWeapons()
{
	/// Notifying weapons
	for (auto it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		printf("Resetting weapon...\n");
		RCSPStream stream;
		stream.addCall(ConfigCodes::Rifle::Functions::rifleReset);
		stream.addCall(ConfigCodes::Rifle::Functions::rifleTurnOn);
		stream.send(*it, true, nullptr);
	}
}

void HeadSensor::turnOffWeapons()
{
	for (auto it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		printf("Turning off weapon\n");
		RCSPStream::remoteCall(*it, ConfigCodes::Rifle::Functions::rifleTurnOff);
	}
}

void HeadSensor::registerWeapon(DeviceAddress weaponAddress)
{
	printf("Registering weapon\n");
	weaponAddress.print();
	auto it = m_weapons.find(weaponAddress);
	if (it == m_weapons.end())
	{
		m_weapons.insert(weaponAddress);
	}

	if (playerState.isAlive())
	{
		RCSPStream::remoteCall(weaponAddress, ConfigCodes::Rifle::Functions::rifleTurnOn);
	} else {
		RCSPStream::remoteCall(weaponAddress, ConfigCodes::Rifle::Functions::rifleTurnOff);
	}

}

/////////////////////
// Test functions
void HeadSensor::testDie(const char*)
{
	playerState.healthCurrent = 0;
	shotCallback(0, 0, 0);
}

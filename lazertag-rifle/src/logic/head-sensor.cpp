/*
 * head-sensor.cpp
 *
 *  Created on: 24 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/head-sensor.hpp"
#include "logic/RCSP-stream.hpp"
#include "dev/console.hpp"
#include "core/scheduler.hpp"
#include "hal/ext-interrupts.hpp"

#include <stdio.h>

HeadSensor::HeadSensor()
{
	m_weapons.insert({1,1,1});
}

void HeadSensor::configure()
{
	IExternalInterruptManager *exti = EXTIS->getEXTI(0);
	exti->init(0);
	exti->turnOn();

	m_mainSensor.setShortMessageCallback(std::bind(&HeadSensor::shotCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	m_mainSensor.init(exti);
	//m_mainSensor.enableDebug(true);

	reset();

	Scheduler::instance().addTask(std::bind(&MilesTag2Receiver::interrogate, &m_mainSensor), false, 50000, 30000);

	//m_mainSensor.enableDebug(true);
	RCSPModem::instance().init();

	printf("Creating console commands for head sensor\n");
	Console::instance().registerCommand(
		"die",
		"kill player",
		std::bind(&HeadSensor::testDie, this, std::placeholders::_1)
	);

	printf("Head sensor ready to use\n");
}

void HeadSensor::shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage)
{
	printf("Cought a shot: team %d, player %d, damage: %d\n", teamId, playerId, damage);
	if (playerState.isAlive()) {
		playerState.damage(damage);
		printf("health: %u, armor: %u\n", playerState.s_health, playerState.s_armor);
		for (auto it = m_weapons.begin(); it != m_weapons.end(); it++)
		{
			RCSPStream stream;
			stream.addValue(ConfigCodes::Player::State::s_health);
			stream.addValue(ConfigCodes::Player::State::s_armor);
			stream.send(*it, true, nullptr);
		}
	}
	if (!playerState.isAlive()) {
		printf("Player died.\n");
		/// Notifying weapons
		for (auto it = m_weapons.begin(); it != m_weapons.end(); it++)
		{
			printf("Turning off weapon...\n");
			RCSPStream stream;
			stream.addCall(ConfigCodes::Rifle::Functions::turnOff);
			stream.send(*it, true, nullptr);
		}
	}
}

void HeadSensor::respawn()
{
	playerState.respawn();
	printf("Player spawned\n");
}

void HeadSensor::reset()
{
	playerState.reset();
	/// Notifying weapons
	for (auto it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		printf("Resetting weapon...\n");
		RCSPStream stream(Package::payloadLength);
		stream.addCall(ConfigCodes::Rifle::Functions::reset);
		stream.addCall(ConfigCodes::Rifle::Functions::turnOn);
		stream.send(*it, true, nullptr);
	}
	printf("Player reseted\n");
}

/////////////////////
// Test functions
void HeadSensor::testDie(const char*)
{
	playerState.s_health = 0;
	shotCallback(0, 0, 0);
}

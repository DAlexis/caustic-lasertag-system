/*
 * rifle.cpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#include "rifle/rifle.hpp"
#include "rcsp/RCSP-stream.hpp"
#include "core/string-utils.hpp"
#include "dev/miles-tag-2.hpp"
#include "dev/console.hpp"
#include "dev/sdcard-fs.hpp"
#include "dev/wav-player.hpp"
#include "hal/system-clock.hpp"

//#include "res/buttons-mapping.h"

#include <stdio.h>

PlayerDisplayableData::PlayerDisplayableData(const DeviceAddress& headSensorAddress) :
	m_headSensorAddress(&headSensorAddress)
{
	healthMax = 0;
	armorMax = 0;

	healthCurrent = 0;
	armorCurrent = 0;

	lifesCountCurrent = 0;
	pointsCount = 0;
	killsCount = 0;
	deathsCount = 0;
}

void PlayerDisplayableData::syncAll()
{
	RCSPMultiStream stream;
	stream.addRequest(ConfigCodes::HeadSensor::Configuration::healthMax);
	stream.addRequest(ConfigCodes::HeadSensor::Configuration::armorMax);
	stream.addRequest(ConfigCodes::HeadSensor::State::healthCurrent);
	stream.addRequest(ConfigCodes::HeadSensor::State::armorCurrent);
	stream.addRequest(ConfigCodes::HeadSensor::State::lifesCountCurrent);
	stream.addRequest(ConfigCodes::HeadSensor::State::pointsCount);
	stream.addRequest(ConfigCodes::HeadSensor::State::killsCount);
	stream.addRequest(ConfigCodes::HeadSensor::State::deathsCount);

	stream.send(*m_headSensorAddress, false);
}

void PlayerDisplayableData::print()
{
	printf("\nCurrent player's state:\n");
	constexpr uint8_t barLength = 10;
	if (healthMax != 0)
	{
		printf("Health:  ");
		printBar(barLength, barLength * healthCurrent / healthMax);
		printf(" %u/%u\n", healthCurrent, healthMax);
	}
	if (armorMax != 0)
	{
		printf("Armor:   ");
		printBar(barLength, barLength * armorCurrent / armorMax);
		printf(" %u/%u\n", armorCurrent, armorMax);
	}
	printf("Lifes:  %u\n", lifesCountCurrent);
	printf("Points: %u\n", pointsCount);
	printf("Kills:  %u\n", killsCount);
	printf("Deaths: %u\n", deathsCount);
}

RifleConfiguration::RifleConfiguration()
{
	setDefault();
}

void RifleConfiguration::setDefault()
{
	slot = 1;
	weightInSlot = 0;

	damageMin = 25;
	damageMax = 25;
	firePeriod = 100000;
	shotDelay = 0;

	jamProb = 0;
	criticalProb = 0;
	criticalCoeff = 1;

	semiAutomaticAllowed = true;
	automaticAllowed = true;

	magazineType = MagazineType::unchangeable;
	reloadAction = ReloadAction::shutterOnly;
	autoReload = ReloadMode::automatic;

	magazinesCountMax = 10;
	magazinesCountStart = magazinesCountMax;

	bulletsInMagazineMax = 30;
	bulletsInMagazineStart = bulletsInMagazineMax;
	reloadingTime = 3000000;

	heatPerShot = 0;
	heatLossPerSec = 0;
}

RifleState::RifleState(RifleConfiguration* config) :
	m_config(config)
{
	if (config)
		reset();
}

void RifleState::reset()
{
	bulletsInMagazineCurrent = m_config->bulletsInMagazineStart;
	magazinesCountCurrent = m_config->magazinesCountStart;
	heatnessCurrent = 0;
	lastReloadTime = 0;
}

Rifle::Rifle()
{
}

void Rifle::configure()
{
	m_fireButton = ButtonsPool::instance().getButtonManager(fireButtonPort, fireButtonPin);
	ButtonsPool::instance().setExti(fireButtonPort, fireButtonPin, true);
	m_fireButton->setAutoRepeat(config.automaticAllowed);
	m_fireButton->setRepeatPeriod(config.firePeriod);
	m_fireButton->setCallback(std::bind(&Rifle::makeShot, this, std::placeholders::_1));
	m_fireButton->turnOn();

	Scheduler::instance().addTask(std::bind(&ButtonManager::interrogate, m_fireButton), false, 5000);

	m_reloadButton = ButtonsPool::instance().getButtonManager(reloadButtonPort, reloadButtonPin);
	m_reloadButton->setAutoRepeat(false);
	m_reloadButton->setRepeatPeriod(2*config.firePeriod);
	m_reloadButton->setCallback(std::bind(&Rifle::reload, this, std::placeholders::_1));
	m_reloadButton->turnOn();

	Scheduler::instance().addTask(std::bind(&ButtonManager::interrogate, m_reloadButton), false, 10000);

	m_automaticFireSwitch = ButtonsPool::instance().getButtonManager(automaticButtonPort, automaticButtonPin);
	m_automaticFireSwitch->turnOff();

	m_semiAutomaticFireSwitch = ButtonsPool::instance().getButtonManager(semiAutomaticButtonPort, semiAutomaticButtonPin);
	m_semiAutomaticFireSwitch->turnOff();


	m_mt2Transmitter.setPlayerId(1);
	m_mt2Transmitter.setTeamId(1);
	m_mt2Transmitter.init();

	printf("- Mounting sd-card\n");
	if (!SDCardFS::instance().init())
		printf("Error during mounting sd-card!\n");


	printf("- Wav player initialization\n");
	WavPlayer::instance().init();

	printf("- Package sender initialization\n");
	RCSPModem::instance().init();

	printf("- Loading default config\n");
	//loadConfig();
	RCSPAggregator::instance().readIni("config.ini");

	Scheduler::instance().addTask(std::bind(&PlayerDisplayableData::print, &playerDisplayable), false, 3000000, 0, 1000);
	Scheduler::instance().addTask(std::bind(&Rifle::updatePlayerState, this), false, 1000000, 0, 1000000);

	//rifleTurnOn();
	// Registering at head sensor's weapons list
	registerWeapon();

	printf("Rifle ready to use\n");
}

void Rifle::loadConfig()
{
	IniParcer *parcer = new IniParcer;
	parcer->setCallback([](const char* key, const char* val) { printf("k = %s, v = %s\n", key, val); });
	Result res = parcer->parseFile("default-config.ini");
	if (!res.isSuccess)
		printf("Error: %s\n", res.errorText);
	delete parcer;
}

void Rifle::makeShot(bool isFirst)
{
	/// @todo Add support of absolutely non-automatic devices
	if (isSafeSwitchSelected())
		return;

	// Preventing reloading while reloading (for those who very like reloading)
	if (isReloading())
		return;

	// Check remaining bullets
	if (state.bulletsInMagazineCurrent == 0)
	{
		/// @todo Play empty magazine sound
		printf("Magazine is empty\n");
		// Disabling auto repeating if automatic mode
		m_fireButton->setAutoRepeat(false);
		return;
	}

	// Check fire mode
	if (!isFirst && !config.automaticAllowed)
		return;

	state.bulletsInMagazineCurrent--;
	m_mt2Transmitter.shot(config.damageMin);
	/// @todo Play shot sound
	WavPlayer::instance().loadFile("sound/shoot-1.wav");
	WavPlayer::instance().play();

	printf("--- shot --->\n");



	if (m_semiAutomaticFireSwitch->state() && config.semiAutomaticAllowed)
		m_fireButton->setAutoRepeat(false);

	if (m_automaticFireSwitch->state() && config.automaticAllowed)
		m_fireButton->setAutoRepeat(true);
}

void Rifle::reload(bool)
{
	uint32_t time = systemClock->getTime();
	// Preventing reloading while reloading (for those who very like reloading)
	/// @todo [low] Do someting to prevent second reloading if user pressed reload key and after much time released with contact bounce
	if (time - state.lastReloadTime < config.reloadingTime)
		return;

	if (state.magazinesCountCurrent == 0)
	{
		/// @todo Play no magazines sound
		return;
	}

	state.lastReloadTime = time;

	/// @todo Play reloading sound
	printf("Reloading...\n");
	// So reloading
	state.magazinesCountCurrent--;
	state.bulletsInMagazineCurrent = config.bulletsInMagazineMax;
	m_fireButton->setAutoRepeat(false);
}

bool Rifle::isSafeSwitchSelected()
{
	return !(m_automaticFireSwitch->state() || m_semiAutomaticFireSwitch->state());
}


bool Rifle::isReloading()
{
	return (systemClock->getTime() - state.lastReloadTime < config.reloadingTime);
}


void Rifle::updatePlayerState()
{
	playerDisplayable.syncAll();
	if (isEnabled && playerDisplayable.healthCurrent == 0)
		rifleTurnOff();

	if (!isEnabled && playerDisplayable.healthCurrent != 0)
		rifleTurnOn();

	//playerDisplayable.print();
}

void Rifle::rifleTurnOff()
{
	printf("Rifle disabled\n");
	isEnabled = false;
	m_fireButton->turnOff();
	m_reloadButton->turnOff();
}

void Rifle::rifleTurnOn()
{
	printf("Rifle enabled\n");
	isEnabled = true;
	m_fireButton->turnOn();
	m_reloadButton->turnOn();
}

void Rifle::rifleReset()
{
	state.reset();
	//turnOn(nullptr, 0);
}

void Rifle::registerWeapon()
{
	printf("Registering weapon...\n");
	RCSPMultiStream stream;
	stream.addCall(ConfigCodes::HeadSensor::Functions::registerWeapon, RCSPModem::instance().devAddr);
	stream.send(config.headSensorAddr, true);
}

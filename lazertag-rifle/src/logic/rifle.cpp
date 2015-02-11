/*
 * rifle.cpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/rifle.hpp"
#include "logic/RCSP-stream.hpp"
#include "core/string-utils.hpp"
#include "dev/miles-tag-2.hpp"
#include "dev/console.hpp"
#include "dev/sdcard-fs.hpp"
#include "dev/wav-player.hpp"
#include "hal/system-clock.hpp"

#include "res/buttons-mapping.h"

#include <stdio.h>

PlayerDisplayableData::PlayerDisplayableData()
{
	health = 0;
	armor = 0;

	s_health = 0;
	s_armor = 0;

	s_lifesCount = 0;
	pointsCount = 0;
	killsCount = 0;
	deathsCount = 0;
}

void PlayerDisplayableData::syncAll()
{
	RCSPMultiStream stream;
	stream.addRequest(ConfigCodes::Player::Configuration::health);
	stream.addRequest(ConfigCodes::Player::Configuration::armor);
	stream.addRequest(ConfigCodes::Player::State::s_health);
	stream.addRequest(ConfigCodes::Player::State::s_armor);
	stream.addRequest(ConfigCodes::Player::State::s_lifesCount);
	stream.addRequest(ConfigCodes::Player::State::pointsCount);
	stream.addRequest(ConfigCodes::Player::State::killsCount);
	stream.addRequest(ConfigCodes::Player::State::deathsCount);

	DeviceAddress target = {1,1,1};
	stream.send(target, false);
}

void PlayerDisplayableData::print()
{
	printf("\nCurrent player's state:\n");
	constexpr uint8_t barLength = 10;
	if (health != 0)
	{
		printf("Health:  ");
		printBar(barLength, barLength * s_health / health);
		printf(" %u/%u\n", s_health, health);
	}
	if (armor != 0)
	{
		printf("Armor:   ");
		printBar(barLength, barLength * s_armor / armor);
		printf(" %u/%u\n", s_armor, armor);
	}
	printf("Lifes:  %u\n", s_lifesCount);
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
	magazinesCount = 10;
	bulletsPerMagazine = 30;
	bulletsInMagazineAtStart = bulletsPerMagazine;
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
	bulletsLeft = m_config->bulletsInMagazineAtStart;
	magazinesLeft = m_config->magazinesCount;
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

	turnOn(nullptr, 0);

	printf("- Mounting sd-card\n");
	if (!SDCardFS::instance().init())
		printf("Error during mounting sd-card!\n");


	printf("- Wav player initialization\n");
	WavPlayer::instance().init();

	printf("- Package sender initialization\n");
	RCSPModem::instance().init();

	printf("- Loading default config\n");
	//loadConfig();
	RCSPAggregator::instance().readIni("default-config.ini");

	Scheduler::instance().addTask(std::bind(&PlayerDisplayableData::syncAll, &playerDisplayable), false, 3000000, 0, 1000);
	Scheduler::instance().addTask(std::bind(&Rifle::updatePlayerState, this), false, 1000000, 0, 1000000);

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
	if (state.bulletsLeft == 0)
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

	state.bulletsLeft--;
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

	if (state.magazinesLeft == 0)
	{
		/// @todo Play no magazines sound
		return;
	}

	state.lastReloadTime = time;

	/// @todo Play reloading sound
	printf("Reloading...\n");
	// So reloading
	state.magazinesLeft--;
	state.bulletsLeft = config.bulletsPerMagazine;
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
	if (isEnabled && playerDisplayable.s_health == 0)
		turnOff(nullptr, 0);

	if (!isEnabled && playerDisplayable.s_health != 0)
		turnOn(nullptr, 0);

	playerDisplayable.print();
}

void Rifle::turnOff(void*, uint16_t)
{
	isEnabled = false;
	m_fireButton->turnOff();
	m_reloadButton->turnOff();
}

void Rifle::turnOn(void*, uint16_t)
{
	isEnabled = true;
	m_fireButton->turnOn();
	m_reloadButton->turnOn();
}

void Rifle::reset(void*, uint16_t)
{
	state.reset();
	//turnOn(nullptr, 0);
}


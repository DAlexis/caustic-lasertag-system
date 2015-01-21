/*
 * rifle.cpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/rifle.hpp"
#include "dev/miles-tag-2.hpp"
#include "dev/console.hpp"
#include "dev/sdcard-fs.hpp"
#include "dev/wav-player.hpp"
#include "hal/system-clock.hpp"

#include "res/buttons-mapping.h"

#include <stdio.h>

template<typename T>
inline void __attribute__((always_inline)) interpretVariable(T& where, void* from)
{
	where = *reinterpret_cast<T*>(from);
}

Rifle::Configuration::Configuration()
{
	setFallback();
}

void Rifle::Configuration::setFallback()
{
	slot = 0;
	weightInSlot = 0;

	damage = 25;
	firePeriod = 100000;
	shotDelay = 0;

	semiAutomaticAllowed = true;
	automaticAllowed = true;

	magazineType = RMT_UNCHANGABLE;
	magazinesReloadMode = RRM_ONLY_SHUTTER;
	autoReload = RAR_ENABLED;
	magazinesCount = 10;
	bulletsPerMagazine = 30;
	bulletsInMagazineAtStart = bulletsPerMagazine;
	reloadingTime = 3000000;
}

Rifle::Rifle()
{
	initState();
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


	if (!SDCardFS::instance().init())
		printf("Error during mounting sd-card!\n");

	WavPlayer::instance().init();
	printf("Rifle ready to use\n");
}

void Rifle::initState()
{
	state.bulletsLeft = config.bulletsInMagazineAtStart;
	state.magazinesLeft = config.magazinesCount;
	state.lastReloadTime = 0;
}

void Rifle::makeShot(bool isFirst)
{
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
	m_mt2Transmitter.shot(config.damage);
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

void Rifle::run()
{
	Scheduler::instance().mainLoop();
}

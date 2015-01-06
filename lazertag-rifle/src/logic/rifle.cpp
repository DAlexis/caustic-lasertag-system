/*
 * rifle.cpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/rifle.hpp"
#include "dev/miles-tag-2.hpp"
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

bool Rifle::Configuration::acceptOption(uint8_t optionCode, void* data)
{
	switch (optionCode)
	{
	case slot_code:                 interpretVariable(slot, data); break;
	case weightInSlot_code:         interpretVariable(weightInSlot, data); break;
	case damage_code:               interpretVariable(damage, data); break;
	case firePeriod_code:           interpretVariable(firePeriod, data); break;
	case shotDelay_code:            interpretVariable(shotDelay, data); break;
	case semiAutomaticAllowed_code: interpretVariable(semiAutomaticAllowed, data); break;
	case automaticAllowed_code:     interpretVariable(automaticAllowed, data); break;
	case magazineType_code:         interpretVariable(magazineType, data); break;
	case magazinesReloadMode_code:  interpretVariable(magazinesReloadMode, data); break;
	case autoReload_code:           interpretVariable(autoReload, data); break;
	case magazinesCount_code:       interpretVariable(magazinesCount, data); break;
	case bulletsInMagazineAtStart_code: interpretVariable(bulletsInMagazineAtStart, data); break;
	case bulletsPerMagazine_code:   interpretVariable(bulletsPerMagazine, data); break;
	case reloadingTime_code:        interpretVariable(reloadingTime, data); break;
	default:
		return false;
	}
	return true;
}

void Rifle::configure()
{
	m_fireButton = ButtonsPool::instance().getButtonManager(fireButtonPort, fireButtonPin);
	ButtonsPool::instance().setExti(fireButtonPort, fireButtonPin, true);
	m_fireButton->setAutoRepeat(config.automaticAllowed);
	m_fireButton->setRepeatPeriod(config.firePeriod);
	m_fireButton->setCallback(std::bind(&Rifle::makeShot, this, std::placeholders::_1));
	m_fireButton->turnOn();

	m_sheduler.addTask(std::bind(&ButtonManager::interrogate, m_fireButton), false, 5000);

	m_reloadButton = ButtonsPool::instance().getButtonManager(reloadButtonPort, reloadButtonPin);
	m_reloadButton->setAutoRepeat(false);
	m_reloadButton->setRepeatPeriod(2*config.firePeriod);
	m_reloadButton->setCallback(std::bind(&Rifle::reload, this, std::placeholders::_1));
	m_reloadButton->turnOn();

	m_sheduler.addTask(std::bind(&ButtonManager::interrogate, m_reloadButton), false, 10000);

	m_mt2Transmitter.setPlayerId(1);
	m_mt2Transmitter.setTeamId(1);
	m_mt2Transmitter.init();
	/*
	devicesPool->getMilesTagTransmitter()->init();
	buttons = devicesPool->buttonsManager();
	// Configuring fire button
	buttons->setButtonCallback(0, makeShotWrapper, this);
	buttons->configButton(
			0,
			//config.automaticAllowed ?
					IButtonsManager::BUTTON_AUTO_REPEAT_ENABLE,
			//		: IButtonsManager::BUTTON_AUTO_REPEAT_DISABLE,
			config.firePeriod
	);
	// Configuring reload button
	/// @todo Configuring reload button

	buttons->setButtonCallback(1, reloadWrapper, this);
	buttons->configButton(
			1,
			IButtonsManager::BUTTON_AUTO_REPEAT_DISABLE,
			config.reloadingTime
	);

	/// @todo Configuring MT transmitter for player and team id
*/

}

void Rifle::initState()
{
	state.bulletsLeft = config.bulletsInMagazineAtStart;
	state.magazinesLeft = config.magazinesCount;
	state.lastReloadTime = 0;
}

void Rifle::makeShot(bool isFirst)
{
	// Preventing reloading while reloading (for those who very like reloading)
	if (isReloading())
		return;

	// Check remaining bullets
	if (state.bulletsLeft == 0)
	{
		/// @todo Play empty magazine sound
		/// @todo Disable button auto repeat
		printf("Magazine is empty\n");
		return;
	}

	// Check fire mode
	if (!isFirst && !config.automaticAllowed)
		return;

	state.bulletsLeft--;
	/// @todo Play shot sound
	printf("--- shot --->\n");

	m_mt2Transmitter.shot(config.damage);
}

void Rifle::reload(bool)
{
	uint32_t time = systemClock->getTime();
	// Preventing reloading while reloading (for those who very like reloading)
	/// @todo [low] Do someting to prevent second reloading if user pressed reload key and after much time released with contact bounce
	if (time - state.lastReloadTime < config.reloadingTime)
		return;

	state.lastReloadTime = time;

	if (state.magazinesLeft == 0)
	{
		/// @todo Play no magazines sound
		return;
	}
	/// @todo Play reloading sound
	printf("Reloading...\n");
	// So reloading
	state.magazinesLeft--;
	state.bulletsLeft = config.bulletsPerMagazine;
}


bool Rifle::isReloading()
{
	return (systemClock->getTime() - state.lastReloadTime < config.reloadingTime);
}

void Rifle::run()
{
	m_sheduler.mainLoop();
}

/*
 * rifle.cpp
 *
 *  Created on: 13 дек. 2014 г.
 *      Author: alexey
 */

#include "rifle.hpp"
#include "utils.hpp"

#include <stdio.h>

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

Rifle::Rifle() :
	m_lastReloadTime(0)
{
}

void Rifle::init()
{
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

	initState();
}

void Rifle::initState()
{
	bulletsLeft = config.bulletsInMagazineAtStart;
	magazinesLeft = config.magazinesCount;
	m_lastReloadTime = 0;
}

void Rifle::mainLoopBody()
{
	buttons->interrogateAllButtons();
}

void Rifle::makeShot(bool isFirst)
{
	// Preventing reloading while reloading (for those who very like reloading)
	if (isReloading())
		return;

	// Check remaining bullets
	if (bulletsLeft == 0)
	{
		/// @todo Play empty magazine sound
		printf("Magazine is empty\n");
		return;
	}

	// Check fire mode
	if (!isFirst && !config.automaticAllowed)
		return;

	bulletsLeft--;
	/// @todo Play shot sound
	printf("--- shot --->\n");
	devicesPool->getMilesTagTransmitter()->shot(config.damage);
}

void Rifle::reload()
{
	uint32_t time = systemTimer.getTime();
	// Preventing reloading while reloading (for those who very like reloading)
	/// @todo [low] Do someting to prevent second reloading if user pressed reload key and after much time released with contact bounce
	if (time - m_lastReloadTime < config.reloadingTime)
		return;

	m_lastReloadTime = time;

	if (magazinesLeft == 0)
	{
		/// @todo Play no magazines sound
		return;
	}
	/// @todo Play reloading sound
	printf("Reloading...\n");
	// So reloading
	magazinesLeft--;
	bulletsLeft = config.bulletsPerMagazine;
}

bool Rifle::isReloading()
{
	return (systemTimer.getTime() - m_lastReloadTime < config.reloadingTime);
}

void Rifle::makeShotWrapper(void* object, bool isFirst)
{
	reinterpret_cast<Rifle*>(object)->makeShot(isFirst);
}

void Rifle::reloadWrapper(void* object, bool)
{
	reinterpret_cast<Rifle*>(object)->reload();
}

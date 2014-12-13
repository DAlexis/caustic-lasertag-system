/*
 * rifle.cpp
 *
 *  Created on: 13 дек. 2014 г.
 *      Author: alexey
 */

#include "rifle.hpp"

Rifle::Configuration::Configuration()
{
	setFallback();
}

void Rifle::Configuration::setFallback()
{
	slot = 0;
	weightInSlot = 0;

	damage = 25;
	firePeriod = 200000;

	magazineType = RMT_UNCHANGABLE;
	magazinesReloadMode = RRM_ONLY_SHUTTER;
	autoReload = RAR_ENABLED;
	magazinesCount = 10;
	bulletsPerMagazine = 30;
}

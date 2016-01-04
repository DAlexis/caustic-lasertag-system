/*
 * player.cpp
 *
 *  Created on: 25 янв. 2015 г.
 *      Author: alexey
 */

#include "head-sensor/resources.hpp"
#include "head-sensor/player-config-and-state.hpp"
#include "core/logging.hpp"

PlayerConfiguration::PlayerConfiguration()
{
	setDefault();
}

void PlayerConfiguration::setDefault()
{
	healthMax = 100;
	armorMax = 100;

	healthStart = 100;
	armorStart = 100;

	armorCoeffStart = 1;
	damageCoeffStart = 1;
	shotsCoeffStart = 1;
	frendlyFireCoeff = 1;
	selfShotCoeff = 0;
	isHealable = 1;

	lifesCount = UINT16_MAX;
	shockDelayImmortal = 500000;
	shockDelayInactive = 500000;
	preRespawnDelay = 0;
	postRespawnDelay = 0;
	autoRespawn = 0;
	plyerId = 1;
	plyerMT2Id = 1;
	teamId = 1;
	slot1MaxWeight = UINT16_MAX;
	slot2MaxWeight = UINT16_MAX;
	slot3MaxWeight = UINT16_MAX;
	slot4MaxWeight = UINT16_MAX;
	slot5MaxWeight = UINT16_MAX;

	zone1DamageCoeff = 1.0;
	zone2DamageCoeff = 1.0;
	zone3DamageCoeff = 1.0;
	zone4DamageCoeff = 1.0;
	zone5DamageCoeff = 1.0;
	zone6DamageCoeff = 1.0;
}

bool PlayerState::damage(uint8_t damage)
{
	UintParameter totalDamage = damage;
	if (healthCurrent == 0)
		return true;

	if (totalDamage > healthCurrent)
	{
		kill();
		return true;
	} else {
		healthCurrent -= totalDamage;
		return false;
	}
}

bool PlayerState::isAlive()
{
	return healthCurrent != 0;
}

void PlayerState::reset()
{
	healthCurrent = m_configuration->healthStart;
	armorCurrent = m_configuration->armorStart;
	armorCoeffCurrent = m_configuration->armorCoeffStart;
	damageCoeffCurrent = m_configuration->damageCoeffStart;
	shotsCoeffCurrent = m_configuration->shotsCoeffStart;
	lifesCountCurrent = m_configuration->lifesCount;

	pointsCount = 0;
	killsCount = 0;
	deathsCount = 0;
	weaponsList.clear();
}

void PlayerState::respawn()
{
	healthCurrent = m_configuration->healthStart;
	armorCurrent = m_configuration->armorStart;
	armorCoeffCurrent = m_configuration->armorCoeffStart;
	damageCoeffCurrent = m_configuration->damageCoeffStart;
	shotsCoeffCurrent = m_configuration->shotsCoeffStart;
	lifesCountCurrent--;
}

void PlayerState::kill()
{
	healthCurrent = 0;
	armorCurrent = 0;
	deathsCount++;
}


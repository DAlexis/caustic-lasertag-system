/*
 * player.cpp
 *
 *  Created on: 25 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/player-config-and-state.hpp"

PlayerConfiguration::PlayerConfiguration()
{
	setDefault();
}

void PlayerConfiguration::setDefault()
{
	health = 100;
	armor = 0;

	armorCoeff = 1;
	damageCoeff = 1;
	shotsCoeff = 1;
	frendlyFireCoeff = 1;
	selfShotCoeff = 0;
	isHealable = 1;

	lifesCount = UINT16_MAX;
	shockDelay = 500000;
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
}

bool PlayerState::damage(uint8_t damage)
{
	UintParameter totalDamage = damage;
	if (totalDamage > health)
	{
		health = 0;
		return true;
	} else {
		health -= totalDamage;
		return false;
	}
}

bool PlayerState::isAlive()
{
	return health != 0;
}

void PlayerState::reset()
{
	health = m_configuration->health;
	armor = m_configuration->armor;
	armorCoeff = m_configuration->armorCoeff;
	damageCoeff = m_configuration->damageCoeff;
	shotsCoeff = m_configuration->shotsCoeff;
	lifesCount = m_configuration->lifesCount;

	pointsCount = 0;
	killsCount = 0;
}

void PlayerState::respawn()
{
	health = m_configuration->health;
	armor = m_configuration->armor;
	armorCoeff = m_configuration->armorCoeff;
	damageCoeff = m_configuration->damageCoeff;
	shotsCoeff = m_configuration->shotsCoeff;
	lifesCount--;
	deathsCount++;
}

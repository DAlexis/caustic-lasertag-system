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
	if (totalDamage > s_health)
	{
		s_health = 0;
		return true;
	} else {
		s_health -= totalDamage;
		return false;
	}
}

bool PlayerState::isAlive()
{
	return s_health != 0;
}

void PlayerState::reset()
{
	s_health = m_configuration->health;
	s_armor = m_configuration->armor;
	s_armorCoeff = m_configuration->armorCoeff;
	s_damageCoeff = m_configuration->damageCoeff;
	s_shotsCoeff = m_configuration->shotsCoeff;
	s_lifesCount = m_configuration->lifesCount;

	pointsCount = 0;
	killsCount = 0;
}

void PlayerState::respawn()
{
	s_health = m_configuration->health;
	s_armor = m_configuration->armor;
	s_armorCoeff = m_configuration->armorCoeff;
	s_damageCoeff = m_configuration->damageCoeff;
	s_shotsCoeff = m_configuration->shotsCoeff;
	s_lifesCount--;
	deathsCount++;
}

/*
 * player.cpp
 *
 *  Created on: 25 янв. 2015 г.
 *      Author: alexey
 */

#include "head-sensor/resources.hpp"
#include "logic/player-config-and-state.hpp"
#include "core/logging.hpp"

//////////////////////
// ConnectedWeaponsList
void ConnectedWeaponsList::deserialize(void* source, OperationSize size)
{
	trace << "Weapons list deserializing\n";
	void* cursor = source;
	uint8_t newSize = 0;
	deserializeAndInc(cursor, newSize);
	if (size != sizeof(newSize) + newSize*sizeof(DeviceAddress))
	{
		error << "Deserializing weapons list: invalid data size\n";
		return;
	}
	weapons.clear();
	for (uint8_t i=0; i<newSize; i++)
	{
		DeviceAddress addr;
		deserializeAndInc(cursor, addr);
		weapons.insert(addr);
	}
}

void ConnectedWeaponsList::serialize(void* destination)
{
	trace << "Weapons list serializing\n";
	void* cursor = destination;
	uint8_t size = weapons.size();

	serializeAndInc(cursor, size);

	for (auto it=weapons.begin(); it != weapons.end(); it++)
	{
		serializeAndInc(cursor, *it);
	}
}

uint32_t ConnectedWeaponsList::serializedSize()
{
	return sizeof(uint8_t) + weapons.size() * sizeof(DeviceAddress);
}


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
	weaponsList.weapons.clear();
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


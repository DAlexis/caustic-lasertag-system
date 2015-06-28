/*
 * player-settings.hpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_

#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/RCSP-modem.hpp"
#include "rcsp/operation-codes.hpp"
#include <set>
#include <stdint.h>

class ConnectedWeaponsList
{
public:
	void deserialize(void* source, OperationSize size);
	void serialize(void* destination);
	uint32_t serializedSize();

	std::set<DeviceAddress> weapons;
};

class PlayerConfiguration
{
public:
	PlayerConfiguration();
	void setDefault();
	/// @todo [Refactor!] Add this values to another state saver or any other way
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, healthMax);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, armorMax);

	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, healthStart);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, armorStart);

	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, armorCoeffStart);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, damageCoeffStart);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, shotsCoeffStart);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, frendlyFireCoeff);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, selfShotCoeff);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, isHealable);

	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, lifesCount);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, uint32_t,      shockDelay);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, preRespawnDelay);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, postRespawnDelay);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, autoRespawn);

	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, plyerId);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, uint8_t, plyerMT2Id);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, uint8_t, teamId);

	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, slot1MaxWeight);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, slot2MaxWeight);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, slot3MaxWeight);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, slot4MaxWeight);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, UintParameter, slot5MaxWeight);

	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, FloatParameter, zone1DamageCoeff);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, FloatParameter, zone2DamageCoeff);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, FloatParameter, zone3DamageCoeff);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, FloatParameter, zone4DamageCoeff);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, FloatParameter, zone5DamageCoeff);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, FloatParameter, zone6DamageCoeff);
};

class PlayerState
{
public:
	PlayerState(PlayerConfiguration* configuration) :
		m_configuration(configuration)
	{
		if (m_configuration)
			reset();
	}

	PAR_ST_R(ConfigCodes::HeadSensor::State, UintParameter, healthCurrent);
	PAR_ST_R(ConfigCodes::HeadSensor::State, UintParameter, armorCurrent);
	PAR_ST_R(ConfigCodes::HeadSensor::State, UintParameter, armorCoeffCurrent);
	PAR_ST_R(ConfigCodes::HeadSensor::State, UintParameter, damageCoeffCurrent);
	PAR_ST_R(ConfigCodes::HeadSensor::State, UintParameter, shotsCoeffCurrent);
	PAR_ST_R(ConfigCodes::HeadSensor::State, UintParameter, lifesCountCurrent);
	PAR_ST_R(ConfigCodes::HeadSensor::State, UintParameter, pointsCount);
	PAR_ST_R(ConfigCodes::HeadSensor::State, UintParameter, killsCount);
	PAR_ST_R(ConfigCodes::HeadSensor::State, UintParameter, deathsCount);

	PAR_CL_SS_R(ConfigCodes::HeadSensor::State, ConnectedWeaponsList, weaponsList);

	/**
	 * Cough some damage and calculate s_armor and s_health after it
	 * @param damage Value of damage
	 * @return true if dead after damage
	 */
	bool damage(uint8_t damage);
	bool isAlive();
	void respawn();
	void reset();
	void kill();

private:
	const PlayerConfiguration* m_configuration = nullptr;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_ */

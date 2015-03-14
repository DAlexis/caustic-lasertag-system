/*
 * player-settings.hpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_

#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/operation-codes.hpp"
#include <stdint.h>

class PlayerConfiguration
{
public:
	PlayerConfiguration();
	void setDefault();
	FUNCION_1P(ConfigCodes::HeadSensor::Functions, PlayerConfiguration, addMaxHealth, int16_t);

	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, healthMax);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, armorMax);

	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, healthStart);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, armorStart);

	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, armorCoeffStart);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, damageCoeffStart);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, shotsCoeffStart);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, frendlyFireCoeff);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, selfShotCoeff);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, isHealable);

	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, lifesCount);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, uint32_t,      shockDelay);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, preRespawnDelay);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, postRespawnDelay);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, autoRespawn);

	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, plyerId);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, uint8_t, plyerMT2Id);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, uint8_t, teamId);

	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, slot1MaxWeight);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, slot2MaxWeight);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, slot3MaxWeight);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, slot4MaxWeight);
	PARAMETER(ConfigCodes::HeadSensor::Configuration, UintParameter, slot5MaxWeight);
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

	PARAMETER(ConfigCodes::HeadSensor::State, UintParameter, healthCurrent);
	PARAMETER(ConfigCodes::HeadSensor::State, UintParameter, armorCurrent);
	PARAMETER(ConfigCodes::HeadSensor::State, UintParameter, armorCoeffCurrent);
	PARAMETER(ConfigCodes::HeadSensor::State, UintParameter, damageCoeffCurrent);
	PARAMETER(ConfigCodes::HeadSensor::State, UintParameter, shotsCoeffCurrent);
	PARAMETER(ConfigCodes::HeadSensor::State, UintParameter, lifesCountCurrent);
	PARAMETER(ConfigCodes::HeadSensor::State, UintParameter, pointsCount);
	PARAMETER(ConfigCodes::HeadSensor::State, UintParameter, killsCount);
	PARAMETER(ConfigCodes::HeadSensor::State, UintParameter, deathsCount);

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

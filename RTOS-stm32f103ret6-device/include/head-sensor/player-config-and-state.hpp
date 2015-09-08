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
#include "head-sensor/head-sensor-base-types.hpp"
#include <stdint.h>

class PlayerConfiguration
{
public:
	PlayerConfiguration();
	void setDefault();
	/// @todo [Refactor!] Add this values to another state saver or any other way
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthMax);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, armorMax);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthStart);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, armorStart);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, armorCoeffStart);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, damageCoeffStart);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shotsCoeffStart);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, frendlyFireCoeff);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, selfShotCoeff);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, isHealable);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, lifesCount);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shockDelayImmortal);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shockDelayInactive);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, preRespawnDelay);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, postRespawnDelay);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, autoRespawn);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, plyerId);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, plyerMT2Id);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, teamId);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot1MaxWeight);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot2MaxWeight);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot3MaxWeight);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot4MaxWeight);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot5MaxWeight);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, zone1DamageCoeff);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, zone2DamageCoeff);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, zone3DamageCoeff);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, zone4DamageCoeff);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, zone5DamageCoeff);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, zone6DamageCoeff);
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

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, healthCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, armorCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, armorCoeffCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, damageCoeffCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, shotsCoeffCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, lifesCountCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, pointsCount);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, killsCount);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, deathsCount);

	PAR_CL_SS(RESTORABLE, ConfigCodes::HeadSensor::State, weaponsList);

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

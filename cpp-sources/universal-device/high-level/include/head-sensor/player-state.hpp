/*
 * player-state.hpp
 *
 *  Created on: 7 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_PLAYER_STATE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_PLAYER_STATE_HPP_

#include "head-sensor/player-config.hpp"

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
	bool respawn();
	void reset();
	void kill();

private:
	const PlayerConfiguration* m_configuration = nullptr;
};


#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_PLAYER_STATE_HPP_ */

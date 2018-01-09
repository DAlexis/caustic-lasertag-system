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
	PlayerState(PlayerConfiguration* configuration, RCSPAggregator &aggregator) :
		m_aggregator(aggregator),
		m_configuration(configuration)
	{
		if (m_configuration)
			reset();
	}

private:
	RCSPAggregator &m_aggregator;

public:

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, healthCurrent, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, armorCurrent, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, armorCoeffCurrent, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, damageCoeffCurrent, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, shotsCoeffCurrent, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, lifesCountCurrent, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::State, deathsCount, m_aggregator);



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

/*
 * player-settings.hpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_

#include "logic/RCSP-aggregator.hpp"
#include "logic/operation-codes.hpp"
#include <stdint.h>

class PlayerConfiguration
{
public:
	PlayerConfiguration();
	void setDefault();

	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, health);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, armor);

	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, armorCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, damageCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, shotsCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, frendlyFireCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, selfShotCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, isHealable);

	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, lifesCount);
	PARAMETER(ConfigCodes::Player::Configuration, uint32_t,      shockDelay);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, preRespawnDelay);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, postRespawnDelay);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, autoRespawn);

	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, plyerId);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, plyerMT2Id);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, teamId);

	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot1MaxWeight);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot2MaxWeight);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot3MaxWeight);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot4MaxWeight);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot5MaxWeight);
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

	PARAMETER(ConfigCodes::Player::State, UintParameter, s_health);
	PARAMETER(ConfigCodes::Player::State, UintParameter, s_armor);
	PARAMETER(ConfigCodes::Player::State, UintParameter, s_armorCoeff);
	PARAMETER(ConfigCodes::Player::State, UintParameter, s_damageCoeff);
	PARAMETER(ConfigCodes::Player::State, UintParameter, s_shotsCoeff);
	PARAMETER(ConfigCodes::Player::State, UintParameter, s_lifesCount);
	PARAMETER(ConfigCodes::Player::State, UintParameter, pointsCount);
	PARAMETER(ConfigCodes::Player::State, UintParameter, killsCount);
	PARAMETER(ConfigCodes::Player::State, UintParameter, deathsCount);

	/**
	 * Cough some damage and calculate s_armor and s_health after it
	 * @param damage Value of damage
	 * @return true if dead after damage
	 */
	bool damage(uint8_t damage);
	bool isAlive();
	void respawn();
	void reset();

private:
	const PlayerConfiguration* m_configuration = nullptr;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_ */

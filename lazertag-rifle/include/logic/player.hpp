/*
 * player-settings.hpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_

#include "logic/configuration.hpp"
#include "logic/config-codes.hpp"
#include <stdint.h>

class PlayerConfiguration
{
public:
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, health);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, armor);

	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, armorCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, damageCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, shotsCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, frendlyFireCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, selfShotCoeff);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, isHealable);

	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, lifesCount);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, shockDelay);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, preRespawnDelay);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, postRespawnDelay);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, autoRespawn);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, plyerId);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot1MaxWeight);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot2MaxWeight);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot3MaxWeight);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot4MaxWeight);
	PARAMETER(ConfigCodes::Player::Configuration, UintParameter, slot5MaxWeight);
};

class PlayerState
{
public:
	PARAMETER(ConfigCodes::Player::State, UintParameter, health);
	PARAMETER(ConfigCodes::Player::State, UintParameter, armor);
	PARAMETER(ConfigCodes::Player::State, UintParameter, armorCoeff);
	PARAMETER(ConfigCodes::Player::State, UintParameter, damageCoeff);
	PARAMETER(ConfigCodes::Player::State, UintParameter, shotsCoeff);
	PARAMETER(ConfigCodes::Player::State, UintParameter, lifesCount);
	PARAMETER(ConfigCodes::Player::State, UintParameter, pointsCount);
	PARAMETER(ConfigCodes::Player::State, UintParameter, killsCount);

	bool damage(uint8_t damage);
	bool isAlive();
};





#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_ */

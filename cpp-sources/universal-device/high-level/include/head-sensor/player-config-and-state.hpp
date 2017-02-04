/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_

#include "rcsp/aggregator.hpp"
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

	//PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, plyerId);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, playerId);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, teamId);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot1MaxWeight);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot2MaxWeight);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot3MaxWeight);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot4MaxWeight);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot5MaxWeight);

	/// @todo Move to PlayerState
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, playerLat);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, playerLon);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, markerColor);

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
	bool respawn();
	void reset();
	void kill();

private:
	const PlayerConfiguration* m_configuration = nullptr;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_ */

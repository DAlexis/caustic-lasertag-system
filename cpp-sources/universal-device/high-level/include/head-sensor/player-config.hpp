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
	PlayerConfiguration(RCSPAggregator& aggregator);
	void setDefault();

private:
	RCSPAggregator& m_aggregator;

public:
	/// @todo [Refactor!] Add this values to another state saver or any other way
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthMax, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, armorMax, m_aggregator);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthStart, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, armorStart, m_aggregator);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, armorCoeffStart, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, damageCoeffStart, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shotsCoeffStart, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, frendlyFireCoeff, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, selfShotCoeff, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, isHealable, m_aggregator);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, lifesCount, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shockDelayImmortal, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, shockDelayInactive, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, preRespawnDelay, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, postRespawnDelay, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, autoRespawn, m_aggregator);

	//PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, plyerId, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, playerId, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, teamId, m_aggregator);

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot1MaxWeight, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot2MaxWeight, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot3MaxWeight, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot4MaxWeight, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, slot5MaxWeight, m_aggregator);

	/// @todo Move to PlayerState
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, playerLat, m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, playerLon, m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, markerColor, m_aggregator);

};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_PLAYER_SETTINGS_HPP_ */

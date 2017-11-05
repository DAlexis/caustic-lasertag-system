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

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_

#include "rcsp/aggregator.hpp"
#include "rcsp/operation-codes.hpp"
#include "network/network-client.hpp"

#include <stdint.h>

class RifleOwnerConfiguration
{
public:
	RifleOwnerConfiguration(RCSPAggregator& aggregator) : m_aggregator(aggregator), playerId(1), teamId(0)
	{}

private:
	RCSPAggregator& m_aggregator;

public:
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, playerId, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, teamId, m_aggregator);
};

class RifleConfiguration
{
public:
	RifleConfiguration(RCSPAggregator& aggregator);

	void setDefault();
	bool isAutoReloading();
	bool isReloadingByDistortingTheBolt();

private:
	RCSPAggregator& m_aggregator;

public:
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, slot, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, weightInSlot, m_aggregator);

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, damageMin, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, damageMax, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, firePeriod, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, shotDelay, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, jamProb, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, criticalProb, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, criticalCoeff, m_aggregator);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, semiAutomaticAllowed, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, automaticAllowed, m_aggregator);
	
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadIsMagazineSmart, m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadNeedMagDisconnect, m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadNeedMagChange, m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadNeedBolt, m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadPlaySound, m_aggregator);

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, magazinesCountStart, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, magazinesCountMax, m_aggregator);

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, bulletsInMagazineStart, m_aggregator);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, bulletsInMagazineMax, m_aggregator);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadingTime, m_aggregator);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, heatPerShot, m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, heatLossPerSec, m_aggregator);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, fireFlashPeriod, m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, fireVibroPeriod, m_aggregator);

	PAR_CL(RESTORABLE, ConfigCodes::Rifle::Configuration, headSensorAddr, m_aggregator);

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, outputPower, m_aggregator);
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_ */

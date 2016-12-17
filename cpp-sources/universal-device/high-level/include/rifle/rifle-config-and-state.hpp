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
	RifleOwnerConfiguration() : playerId(1), teamId(0)
	{}

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, playerId);
	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, teamId);
};

class RifleConfiguration
{
public:
	RifleConfiguration();

	void setDefault();
	bool isAutoReloading();
	bool isReloadingByDistortingTheBolt();

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, slot);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, weightInSlot);

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, damageMin);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, damageMax);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, firePeriod);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, shotDelay);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, jamProb);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, criticalProb);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, criticalCoeff);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration,semiAutomaticAllowed);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, automaticAllowed);
	
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadIsMagazineSmart);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadNeedMagDisconnect);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadNeedMagChange);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadNeedBolt);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadPlaySound);

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, magazinesCountStart);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, magazinesCountMax);

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, bulletsInMagazineStart);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, bulletsInMagazineMax);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, reloadingTime);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, heatPerShot);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, heatLossPerSec);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, fireFlashPeriod);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, fireVibroPeriod);

	PAR_CL(RESTORABLE, ConfigCodes::Rifle::Configuration, headSensorAddr);

	PAR_ST(RESTORABLE, ConfigCodes::Rifle::Configuration, outputPower);
};

class RifleState
{
public:
	RifleState(RifleConfiguration* config);
	void reset();
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::State, bulletsInMagazineCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::State, magazinesCountCurrent);
	PAR_ST(RESTORABLE, ConfigCodes::Rifle::State, heatnessCurrent);

	uint32_t lastReloadTime;
	bool isEnabled = false;
	bool isHSConnected = false;
	const RifleConfiguration& config() { return *m_config; }

private:
	RifleConfiguration* m_config = nullptr;
};


class PlayerPartialState
{
public:
	PlayerPartialState(const DeviceAddress& headSensorAddress, OrdinaryNetworkClient* networkClient);
	void syncAll();
	void print();
	bool isAlive();

	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, healthMax);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::Configuration, armorMax);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, healthCurrent);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, armorCurrent);

	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, lifesCountCurrent);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, pointsCount);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, killsCount);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::HeadSensor::State, deathsCount);

private:
	const DeviceAddress* m_headSensorAddress;
	OrdinaryNetworkClient* m_networkClient;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_ */

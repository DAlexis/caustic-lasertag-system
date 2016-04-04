/*
 * rifle-config-and-state.hpp
 *
 *  Created on: 08 февр. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_

#include <stdint.h>
#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/operation-codes.hpp"

class RifleOwnerConfiguration
{
public:
	RifleOwnerConfiguration() : plyerMT2Id(1), teamId(0)
	{}

	PAR_ST(RESTORABLE, ConfigCodes::HeadSensor::Configuration, plyerMT2Id);
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

	PAR_CL(NOT_RESTORABLE, ConfigCodes::Rifle::Configuration, headSensorAddr);

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
	const RifleConfiguration& config() { return *m_config; }

private:
	RifleConfiguration* m_config = nullptr;
};


class PlayerPartialState
{
public:
	PlayerPartialState(const DeviceAddress& headSensorAddress);
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
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_ */

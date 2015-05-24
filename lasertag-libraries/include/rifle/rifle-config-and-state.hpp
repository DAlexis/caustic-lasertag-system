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
#include "rcsp/RCSP-modem.hpp"

class RifleOwnerConfiguration
{
public:
	RifleOwnerConfiguration() : plyerMT2Id(1), teamId(0)
	{}

	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, uint8_t, plyerMT2Id);
	PAR_ST_R(ConfigCodes::HeadSensor::Configuration, uint8_t, teamId);
};

class RifleConfiguration
{
public:
	RifleConfiguration();

	void setDefault();
	bool isAutoReloading();
	bool isReloadingByDistortingTheBolt();

	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, slot);
	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, weightInSlot);

	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, damageMin);
	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, damageMax);
	PAR_ST_R(ConfigCodes::Rifle::Configuration, uint32_t , firePeriod);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, shotDelay);
	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, jamProb);
	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, criticalProb);
	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, criticalCoeff);

	PAR_ST(ConfigCodes::Rifle::Configuration, bool, semiAutomaticAllowed);
	PAR_ST_R(ConfigCodes::Rifle::Configuration, bool, automaticAllowed);

	
	PAR_ST(ConfigCodes::Rifle::Configuration, bool, reloadIsMagazineSmart);
	PAR_ST(ConfigCodes::Rifle::Configuration, bool, reloadNeedMagDisconnect);
	PAR_ST(ConfigCodes::Rifle::Configuration, bool, reloadNeedMagChange);
	PAR_ST(ConfigCodes::Rifle::Configuration, bool, reloadNeedBolt);
	PAR_ST(ConfigCodes::Rifle::Configuration, bool, reloadPlaySound);

	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, magazinesCountStart);
	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, magazinesCountMax);

	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, bulletsInMagazineStart);
	PAR_ST_R(ConfigCodes::Rifle::Configuration, UintParameter, bulletsInMagazineMax);

	PAR_ST(ConfigCodes::Rifle::Configuration, uint32_t, reloadingTime);

	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, heatPerShot);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, heatLossPerSec);

	PAR_CL(ConfigCodes::Rifle::Configuration, DeviceAddress, headSensorAddr);
};

class RifleState
{
public:
	RifleState(RifleConfiguration* config);
	void reset();
	PAR_ST_R(ConfigCodes::Rifle::State, UintParameter, bulletsInMagazineCurrent);
	PAR_ST_R(ConfigCodes::Rifle::State, UintParameter, magazinesCountCurrent);
	PAR_ST_R(ConfigCodes::Rifle::State, UintParameter, heatnessCurrent);

	uint32_t lastReloadTime;

private:
	RifleConfiguration* m_config = nullptr;
};





#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_ */

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

	PAR_ST(ConfigCodes::HeadSensor::Configuration, uint8_t, plyerMT2Id);
	PAR_ST(ConfigCodes::HeadSensor::Configuration, uint8_t, teamId);
};

class RifleConfiguration
{
public:
	RifleConfiguration();

	void setDefault();

	struct ReloadAction
	{
		constexpr static uint8_t shutterOnly        = 0;  ///< Disort the shutter
		constexpr static uint8_t magazineAndShutter = 1;  ///< Changing magazine (depends on RifleMagazineType) and disort the shutter
	};

	struct ReloadMode
	{
		constexpr static uint8_t manual = 0;
		constexpr static uint8_t automatic = 1;
	};

	struct MagazineType
	{
		constexpr static uint8_t unchangeable   = 0;    ///< Magazine does not really exists
		constexpr static uint8_t oneReplaceable = 1;    ///< One magazine that can be disconnected
		constexpr static uint8_t twoReplaceable = 2;    ///< Two magazines that should be swapped
		constexpr static uint8_t intellectual   = 3;    ///< Magazines contains MCU and interface
	};

	struct FireMode
	{
		constexpr static uint8_t single        = 0;     ///< Disort the shutter after every shot
		constexpr static uint8_t semiAutomatic = 1;     ///< One press - one shot and no more
		constexpr static uint8_t Automatic     = 2;     ///< One press - many shots
	};

	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, slot);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, weightInSlot);

	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, damageMin);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, damageMax);
	PAR_ST(ConfigCodes::Rifle::Configuration, uint32_t , firePeriod);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, shotDelay);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, jamProb);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, criticalProb);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, criticalCoeff);

	PAR_ST(ConfigCodes::Rifle::Configuration, bool, semiAutomaticAllowed);
	PAR_ST(ConfigCodes::Rifle::Configuration, bool, automaticAllowed);

	PAR_ST(ConfigCodes::Rifle::Configuration, uint8_t, magazineType);
	PAR_ST(ConfigCodes::Rifle::Configuration, uint8_t, reloadAction);
	PAR_ST(ConfigCodes::Rifle::Configuration, uint8_t, autoReload);

	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, magazinesCountStart);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, magazinesCountMax);

	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, bulletsInMagazineStart);
	PAR_ST(ConfigCodes::Rifle::Configuration, UintParameter, bulletsInMagazineMax);

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
	PAR_ST(ConfigCodes::Rifle::State, UintParameter, bulletsInMagazineCurrent);
	PAR_ST(ConfigCodes::Rifle::State, UintParameter, magazinesCountCurrent);
	PAR_ST(ConfigCodes::Rifle::State, UintParameter, heatnessCurrent);

	uint32_t lastReloadTime;

private:
	RifleConfiguration* m_config = nullptr;
};





#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_ */

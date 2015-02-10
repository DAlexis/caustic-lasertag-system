/*
 * rifle-config-and-state.hpp
 *
 *  Created on: 08 февр. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_

#include <stdint.h>
#include "logic/RCSP-aggregator.hpp"
#include "logic/operation-codes.hpp"

class RifleConfiguration
{
public:
	RifleConfiguration();

	void setDefault();

	struct ReloadAction
	{
		constexpr static uint8_t shutterOnly        = 0;  // Disort the shutter
		constexpr static uint8_t magazineAndShutter = 1;  // Changing magazine (depends on RifleMagazineType) and disort the shutter
	};

	struct ReloadMode
	{
		constexpr static uint8_t manual = 0;
		constexpr static uint8_t automatic = 1;
	};

	struct MagazineType
	{
		constexpr static uint8_t unchangeable   = 0;    // Magazine does not really exists
		constexpr static uint8_t oneReplaceable = 1;    // One magazine that can be disconnected
		constexpr static uint8_t twoReplaceable = 2;    // Two magazines that should be swapped
		constexpr static uint8_t intellectual   = 3;    // Magazines contains MCU and interface
	};

	struct FireMode
	{
		constexpr static uint8_t single        = 0;     // Disort the shutter after every shot
		constexpr static uint8_t semiAutomatic = 1;     // One press - one shot and no more
		constexpr static uint8_t Automatic     = 2;     // One press - many shots
	};

	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, slot);
	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, weightInSlot);

	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, damageMin);
	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, damageMax);
	PARAMETER(ConfigCodes::Rifle::Configuration, uint32_t , firePeriod);
	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, shotDelay);
	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, jamProb);
	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, criticalProb);
	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, criticalCoeff);

	PARAMETER(ConfigCodes::Rifle::Configuration, bool, semiAutomaticAllowed);
	PARAMETER(ConfigCodes::Rifle::Configuration, bool, automaticAllowed);

	PARAMETER(ConfigCodes::Rifle::Configuration, uint8_t, magazineType);
	PARAMETER(ConfigCodes::Rifle::Configuration, uint8_t, reloadAction);
	PARAMETER(ConfigCodes::Rifle::Configuration, uint8_t, autoReload);

	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, magazinesCount);
	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, bulletsInMagazineAtStart);
	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, bulletsPerMagazine);
	PARAMETER(ConfigCodes::Rifle::Configuration, uint32_t, reloadingTime);

	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, heatPerShot);
	PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, heatLossPerSec);
};

class RifleState
{
public:
	RifleState(RifleConfiguration* config);
	void reset();
	PARAMETER(ConfigCodes::Rifle::State, UintParameter, bulletsLeft);
	PARAMETER(ConfigCodes::Rifle::State, UintParameter, magazinesLeft);

	uint32_t lastReloadTime;

private:
	RifleConfiguration* m_config = nullptr;
};





#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_CONFIG_AND_STATE_HPP_ */

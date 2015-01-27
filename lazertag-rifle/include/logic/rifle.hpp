/*
 * rifle.hpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_

#include "logic/configuration.hpp"
#include "logic/config-codes.hpp"
#include "logic/device.hpp"
#include "logic/package-former.hpp"
#include "dev/buttons.hpp"
#include "dev/miles-tag-2.hpp"
#include "core/scheduler.hpp"

#include <stdint.h>


enum RifleReloadMode
{
	RRM_ONLY_SHUTTER,          // Disort the shutter
	RRM_MAGAZINE_AND_SHUTTER,  // Changing magazine (depends on RifleMagazineType) and disort the shutter
};

enum RifleAutoReload
{
	RAR_DISABLED = 0,
	RAR_ENABLED
};

enum RifleMagazineType
{
	RMT_UNCHANGABLE = 0,    // Magazine does not really exists
	RMT_ONE_REPLACEABLE,    // One magazine that can be disconnected
	RMT_TWO_REPLACEABLE,    // Two magazines that should be swapped
	RMT_INTELLECTUAL,       // Magazines contains MCU and interface
};

enum FireMode
{
	FM_SINGLE = 0,          // Disort the shutter after every shot
	FM_SEMI_AUTOMATIC = 1,  // One press - one shot and no more
	FM_AUTOMATIC = 2
};

class Rifle
{
public:
	class Configuration
	{
	public:
		Configuration();

		void setFallback();

		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, slot);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, weightInSlot);

		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, damageMin);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, damageMax);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, firePeriod);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, shotDelay);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, jamProb);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, criticalProb);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, criticalCoeff);

		PARAMETER(ConfigCodes::Rifle::Configuration, bool, semiAutomaticAllowed);
		PARAMETER(ConfigCodes::Rifle::Configuration, bool, automaticAllowed);

		PARAMETER(ConfigCodes::Rifle::Configuration, RifleMagazineType, magazineType);
		PARAMETER(ConfigCodes::Rifle::Configuration, RifleReloadMode, magazinesReloadMode);
		PARAMETER(ConfigCodes::Rifle::Configuration, RifleAutoReload, autoReload);

		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, magazinesCount);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, bulletsInMagazineAtStart);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, bulletsPerMagazine);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, reloadingTime);

		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, heatPerShot);
		PARAMETER(ConfigCodes::Rifle::Configuration, UintParameter, heatLossPerSec);
	};

	class State
	{
	public:
		PARAMETER(ConfigCodes::Rifle::State, UintParameter, bulletsLeft);
		PARAMETER(ConfigCodes::Rifle::State, UintParameter, magazinesLeft);

		uint32_t lastReloadTime;
	};

	Rifle();

	void configure();

	Configuration config;
	State state;
	DeviceParameters device;

private:
	void initState();
	void makeShot(bool isFirst);
	void reload(bool isFirst);

	bool isReloading();
	bool isSafeSwitchSelected();

	ButtonManager* m_fireButton = nullptr;
	ButtonManager* m_reloadButton = nullptr;
	ButtonManager* m_automaticFireSwitch = nullptr;
	ButtonManager* m_semiAutomaticFireSwitch = nullptr;

	MilesTag2Transmitter m_mt2Transmitter;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_ */

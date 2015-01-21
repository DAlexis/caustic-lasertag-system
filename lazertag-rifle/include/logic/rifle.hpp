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

		PARAMETER(ConfigCodes::Rifle, uint32_t, slot);
		PARAMETER(ConfigCodes::Rifle, uint32_t, weightInSlot);

		PARAMETER(ConfigCodes::Rifle, uint32_t, damage);
		PARAMETER(ConfigCodes::Rifle, uint32_t, firePeriod);
		PARAMETER(ConfigCodes::Rifle, uint32_t, shotDelay);

		PARAMETER(ConfigCodes::Rifle, bool, semiAutomaticAllowed);
		PARAMETER(ConfigCodes::Rifle, bool, automaticAllowed);

		PARAMETER(ConfigCodes::Rifle, RifleMagazineType, magazineType);
		PARAMETER(ConfigCodes::Rifle, RifleReloadMode, magazinesReloadMode);
		PARAMETER(ConfigCodes::Rifle, RifleAutoReload, autoReload);

		PARAMETER(ConfigCodes::Rifle, uint32_t, magazinesCount);
		PARAMETER(ConfigCodes::Rifle, uint32_t, bulletsInMagazineAtStart);
		PARAMETER(ConfigCodes::Rifle, uint32_t, bulletsPerMagazine);
		PARAMETER(ConfigCodes::Rifle, uint32_t, reloadingTime);
	};

	class State
	{
	public:
		PARAMETER(ConfigCodes::Rifle, uint32_t, bulletsLeft);
		PARAMETER(ConfigCodes::Rifle, uint32_t, magazinesLeft);
		PARAMETER(ConfigCodes::Rifle, uint32_t, lastReloadTime);
	};

	Rifle();

	void configure();
	void run();

	Configuration config;
	State state;

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

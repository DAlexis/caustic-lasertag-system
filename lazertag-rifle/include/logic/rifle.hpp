/*
 * rifle.hpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_

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
		bool acceptOption(uint8_t optionCode, void* data);    /// @return true if option is accepted
		//void readFromFile(const char* filename);

		constexpr static uint8_t slot_code = 1;
		uint32_t slot;
		constexpr static uint8_t weightInSlot_code = 2;
		uint32_t weightInSlot;

		constexpr static uint8_t damage_code = 5;
		uint32_t damage;
		constexpr static uint8_t firePeriod_code = 6;
		uint32_t firePeriod; // Time between shots in us
		constexpr static uint8_t shotDelay_code = 7;
		uint32_t shotDelay;  // Delay between shutter pressing and shot

		constexpr static uint8_t semiAutomaticAllowed_code = 10;
		bool semiAutomaticAllowed;
		constexpr static uint8_t automaticAllowed_code = 11;
		bool automaticAllowed;

		constexpr static uint8_t magazineType_code = 15;
		RifleMagazineType magazineType;
		constexpr static uint8_t magazinesReloadMode_code = 16;
		RifleReloadMode magazinesReloadMode;
		constexpr static uint8_t autoReload_code = 17;
		RifleAutoReload autoReload;

		constexpr static uint8_t magazinesCount_code = 20;
		uint32_t magazinesCount;
		constexpr static uint8_t bulletsInMagazineAtStart_code = 21;
		uint32_t bulletsInMagazineAtStart;
		constexpr static uint8_t bulletsPerMagazine_code = 22;
		uint32_t bulletsPerMagazine;
		constexpr static uint8_t reloadingTime_code = 23;
		uint32_t reloadingTime;
	};

	class State
	{
	public:
		constexpr static uint8_t bulletsLeft_code = 101;
		uint32_t bulletsLeft = 0;
		constexpr static uint8_t magazinesLeft_code = 102;
		uint32_t magazinesLeft = 0;
		constexpr static uint8_t lastReloadTime_code = 103;
		uint32_t lastReloadTime = 0;
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

	Scheduler m_sheduler;

	ButtonManager* m_fireButton = nullptr;
	ButtonManager* m_reloadButton = nullptr;
	ButtonManager* m_automaticFireSwitch = nullptr;
	ButtonManager* m_semiAutomaticFireSwitch = nullptr;

	MilesTag2Transmitter m_mt2Transmitter;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_RIFLE_HPP_ */

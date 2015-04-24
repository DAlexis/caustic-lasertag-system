/*
 * rifle.hpp
 *
 *  Created on: 13 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_RIFLE_HPP_
#define LAZERTAG_DEVICE_INCLUDE_RIFLE_HPP_

#include "hal/abstract-devices-pool.hpp"
#include <stdint.h>

/*
enum RifleMagazinesCountMode
{
	RMU_FIXED_COUNT = 0,       // Fixed count when respawn
	RMU_UNLIMITED,             // Unlimited
};
*/
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
	FM_SINGLE = 0,        // Disort the shutter after every shot
	FM_SEMI_AUTOMATIC = 1,    // One press - one shot and no more
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
		//void readFromFile(const char* filename);

		uint32_t slot;
		uint32_t weightInSlot;

		uint32_t damage;
		uint32_t firePeriod; // Time between shots in us
		bool semiAutomaticAllowed;
		bool automaticAllowed;

		RifleMagazineType magazineType;
		RifleReloadMode magazinesReloadMode;
		RifleAutoReload autoReload;
		uint32_t magazinesCount;
		uint32_t bulletsInMagazineAtStart;
		uint32_t bulletsPerMagazine;
		uint32_t reloadingTime;

	};

	Rifle();
	void init();
	void mainLoopBody();

	Configuration config;

private:
	void makeShot(bool isFirst);
	void reload();
	bool isReloading();

	void initState();
	void configureButtons();


	static void makeShotWrapper(void* object, bool isFirst);
	static void reloadWrapper(void* object, bool isFirst);

	uint32_t bulletsLeft;
	uint32_t magazinesLeft;
	IButtonsManager* buttons;

	uint32_t m_lastReloadTime;
	//IMilesTag2Transmitter* mt2;
};





#endif /* LAZERTAG_DEVICE_INCLUDE_RIFLE_HPP_ */

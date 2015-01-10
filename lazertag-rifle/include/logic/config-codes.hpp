/*
 * config-codes.h
 *
 *  Created on: 10 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LOGIC_CONFIGS_H_
#define LOGIC_CONFIGS_H_

#include "logic/configuration.hpp"
#include "core/macro-utils.hpp"

#define PAR_CODE(variable, value)   constexpr uint16_t variable = SetParameterOC(value); \
                                    constexpr const char variable##Text[] = STRINGIFICATE(variable);

namespace ConfigCodes
{
	namespace Rifle
	{
		// Rifle configuration
		PAR_CODE(slot,         1)
		PAR_CODE(weightInSlot, 2)
		PAR_CODE(damage,       5)
		PAR_CODE(firePeriod,   6)
		PAR_CODE(shotDelay,    7)

		PAR_CODE(semiAutomaticAllowed, 10)
		PAR_CODE(automaticAllowed,     11)

		PAR_CODE(magazineType,         15)
		PAR_CODE(magazinesReloadMode,  16)
		PAR_CODE(autoReload,           17)

		PAR_CODE(magazinesCount,           20)
		PAR_CODE(bulletsInMagazineAtStart, 21)
		PAR_CODE(bulletsPerMagazine,       22)
		PAR_CODE(reloadingTime,            23)

		// Rifle state
		PAR_CODE(bulletsLeft,             101)
		PAR_CODE(magazinesLeft,           102)
		PAR_CODE(lastReloadTime,          103)
	}
}



#endif /* LOGIC_CONFIGS_H_ */

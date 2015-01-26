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

using UintParameter = uint16_t;
using PlayerId = uint8_t[3];


namespace ConfigCodes
{
	namespace Device
	{
		PAR_CODE(devAddr,           2000)
	}
	namespace Rifle
	{
		namespace Configuration
		{
			PAR_CODE(slot,           1)
			PAR_CODE(weightInSlot,   2)
			PAR_CODE(damageMin,      5)
			PAR_CODE(damageMax,      6)
			PAR_CODE(firePeriod,     7)
			PAR_CODE(shotDelay,      8)
			PAR_CODE(jamProb,        9)
			PAR_CODE(criticalProb,   10)
			PAR_CODE(criticalCoeff,  11)

			PAR_CODE(semiAutomaticAllowed, 11)
			PAR_CODE(automaticAllowed,     12)

			PAR_CODE(magazineType,         15)
			PAR_CODE(magazinesReloadMode,  16)
			PAR_CODE(autoReload,           17)

			PAR_CODE(magazinesCount,           20)
			PAR_CODE(bulletsInMagazineAtStart, 21)
			PAR_CODE(bulletsPerMagazine,       22)
			PAR_CODE(reloadingTime,            23)

			PAR_CODE(heatPerShot,              30)
			PAR_CODE(heatLossPerSec,           31)
		}

		namespace State
		{
			// Rifle state
			PAR_CODE(bulletsLeft,             101)
			PAR_CODE(magazinesLeft,           102)
		}
	}

	namespace Player
	{
		namespace Configuration
		{
			/// Player configuration
			PAR_CODE(health,      1000)
			PAR_CODE(armor,       1001)

			/// Effectivity of armor
			PAR_CODE(armorCoeff,  1002)

			/// Simply multiplied by damage to player
			PAR_CODE(damageCoeff, 1003)

			/// Multiplied by player's shutting damage
			PAR_CODE(shotsCoeff,  1004)

			/// Multiplied by friendly fire
			PAR_CODE(frendlyFireCoeff,      1005)

			PAR_CODE(selfShotCoeff,         1006)

			PAR_CODE(isHealable,   1007)
			PAR_CODE(lifesCount,   1008)


			PAR_CODE(shockDelay,        1010)
			PAR_CODE(preRespawnDelay,   1011)
			PAR_CODE(postRespawnDelay,  1012)
			PAR_CODE(autoRespawn,       1013)

			PAR_CODE(plyerId,        1030)

			PAR_CODE(slot1MaxWeight,    1041)
			PAR_CODE(slot2MaxWeight,    1042)
			PAR_CODE(slot3MaxWeight,    1043)
			PAR_CODE(slot4MaxWeight,    1044)
			PAR_CODE(slot5MaxWeight,    1045)
		}

		namespace State
		{
			PAR_CODE(health,      1100)
			PAR_CODE(armor,       1101)
			PAR_CODE(armorCoeff,  1102)
			PAR_CODE(damageCoeff, 1103)
			PAR_CODE(shotsCoeff,  1104)

			PAR_CODE(lifesCount,  1005)

			PAR_CODE(pointsCount, 1110)
			PAR_CODE(killsCount,  1111)
		}
	}
}


#endif /* LOGIC_CONFIGS_H_ */

/*
 * config-codes.h
 *
 *  Created on: 10 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LOGIC_CONFIGS_H_
#define LOGIC_CONFIGS_H_

#include "rcsp/RCSP-aggregator.hpp"
#include "core/macro-utils.hpp"

#define PAR_CODE(variable, value)   constexpr uint16_t variable = RCSPAggregator::SetObjectOC(value); \
                                    constexpr const char variable##Text[] = STRINGIFICATE(variable);

#define FUNC_CODE(function, value)  constexpr uint16_t function = RCSPAggregator::SetCallRequestOC(value);

/*
 *
 * Variables naming guideline:
 *    variableMax
 *    variableMin
 *    variableStart
 *    variableCurrent
 */


using UintParameter = uint16_t;
using FloatParameter = float;
using PlayerId = uint8_t[3];

namespace ConfigCodes
{
	constexpr uint16_t noOperation = 0;
	constexpr uint16_t acknoledgement = RCSPAggregator::SetCallRequestOC(0xFFFF);
	namespace AnyDevice
	{
		namespace Configuration
		{
			PAR_CODE(devAddr,           2000)
		}

		namespace Functions
		{
			FUNC_CODE(resetToDefaults,   2100)
		}
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
			PAR_CODE(reloadAction,         16)
			PAR_CODE(autoReload,           17)

			PAR_CODE(magazinesCountStart,      20)
			PAR_CODE(magazinesCountMax,        21)

			PAR_CODE(bulletsInMagazineStart,   22)
			PAR_CODE(bulletsInMagazineMax,     23)

			PAR_CODE(reloadingTime,            24)

			PAR_CODE(heatPerShot,              30)
			PAR_CODE(heatLossPerSec,           31)

			PAR_CODE(headSensorAddr,           100)
		}

		namespace State
		{
			// Rifle state
			PAR_CODE(bulletsInMagazineCurrent, 101)
			PAR_CODE(magazinesCountCurrent,    102)

			PAR_CODE(heatnessCurrent,    110)
		}

		namespace Functions
		{
			FUNC_CODE(rifleTurnOff,       201)
			FUNC_CODE(rifleTurnOn,        202)
			FUNC_CODE(rifleReset,         203)
			FUNC_CODE(rifleRespawn,       204)
			FUNC_CODE(rifleDie,           205)
		}
	}

	namespace HeadSensor
	{
		namespace Configuration
		{
			/// Player configuration
			PAR_CODE(healthMax,      1000)
			PAR_CODE(armorMax,       1001)
			PAR_CODE(healthStart,       1003)
			PAR_CODE(armorStart,       1004)

			PAR_CODE(isHealable,   1010)
			PAR_CODE(lifesCount,   1011)

			/// Effectivity of armor
			PAR_CODE(armorCoeffStart,  1012)

			/// Simply multiplied by damage to player
			PAR_CODE(damageCoeffStart, 1013)

			/// Multiplied by player's shutting damage
			PAR_CODE(shotsCoeffStart,  1014)

			/// Multiplied by friendly fire
			PAR_CODE(frendlyFireCoeff,      1015)

			PAR_CODE(selfShotCoeff,         1016)

			PAR_CODE(shockDelay,        1020)
			PAR_CODE(preRespawnDelay,   1021)
			PAR_CODE(postRespawnDelay,  1022)
			PAR_CODE(autoRespawn,       1023)

			PAR_CODE(plyerId,        1030)
			PAR_CODE(plyerMT2Id,     1031)
			PAR_CODE(teamId,         1032)

			PAR_CODE(slot1MaxWeight,    1041)
			PAR_CODE(slot2MaxWeight,    1042)
			PAR_CODE(slot3MaxWeight,    1043)
			PAR_CODE(slot4MaxWeight,    1044)
			PAR_CODE(slot5MaxWeight,    1045)


			PAR_CODE(zone1DamageCoeff,    1500)
			PAR_CODE(zone2DamageCoeff,    1501)
			PAR_CODE(zone3DamageCoeff,    1502)
			PAR_CODE(zone4DamageCoeff,    1503)
			PAR_CODE(zone5DamageCoeff,    1504)
			PAR_CODE(zone6DamageCoeff,    1505)
		}

		namespace State
		{
			PAR_CODE(healthCurrent,      1100)
			PAR_CODE(armorCurrent,       1101)
			PAR_CODE(armorCoeffCurrent,  1102)
			PAR_CODE(damageCoeffCurrent, 1103)
			PAR_CODE(shotsCoeffCurrent,  1104)

			PAR_CODE(lifesCountCurrent,  1005)

			PAR_CODE(pointsCount, 1110)
			PAR_CODE(killsCount,  1111)
			PAR_CODE(deathsCount, 1112)

			PAR_CODE(weaponsList, 1200)
		}

		namespace Functions
		{
			FUNC_CODE(playerTurnOff,      1201)
			FUNC_CODE(playerTurnOn,       1202)
			FUNC_CODE(playerReset,        1203)
			FUNC_CODE(playerRespawn,      1204)
			FUNC_CODE(playerKill,         1205)

			FUNC_CODE(addMaxHealth,       1220)
			FUNC_CODE(setTeam,            1221)

			FUNC_CODE(registerWeapon,     1300)
		}
	}
}


#endif /* LOGIC_CONFIGS_H_ */

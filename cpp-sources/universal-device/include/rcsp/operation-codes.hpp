/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#ifndef LOGIC_CONFIGS_H_
#define LOGIC_CONFIGS_H_

#include "rcsp/base-types.hpp"
#include "rcsp/RCSP-codes-manipulation.hpp"

// Includes that contains types which might be used as parameters types
#include "device/device-base-types.hpp"
#include "rcsp/RCSP-base-types.hpp"
#include "network/network-base-types.hpp"
#include "rifle/rifle-base-types.hpp"
#include "head-sensor/head-sensor-base-types.hpp"
#include "ir/MT2-base-types.hpp"
#include "game/game-base-types.hpp"

/*
 *
 * Variables naming guideline:
 *    variableMax
 *    variableMin
 *    variableStart
 *    variableCurrent
 */

namespace ConfigCodes
{
	constexpr uint16_t noOperation = 0;
	//constexpr uint16_t acknoledgement = RCSPCodeManipulator::makeCallRequest(0xFFFF);
	namespace AnyDevice
	{
		namespace Configuration
		{
			PAR_CODE(DeviceAddress, devAddr,           2000)
			PAR_CODE(DeviceName,    deviceName,        2001)
			PAR_CODE(UintParameter, deviceType,        2002)

			PAR_CODE(FloatParameter, supplyVoltageMin, 2010)
			PAR_CODE(FloatParameter, supplyVoltageMax, 2011)
			PAR_CODE(FloatParameter, adcCalibrationCoeff, 2013)
			PAR_CODE(UintParameter, lcdContrast,       2020)
		}

		namespace Functions
		{
			FUNC_CODE_NP(resetToDefaults,   2100)
		}

		namespace State
		{
			PAR_CODE(FloatParameter, supplyVoltage,    2015)
			PAR_CODE(UintParameter,  chargePercent,    2016)
		}
	}

	namespace Rifle
	{
		namespace Configuration
		{
			PAR_CODE(UintParameter, slot,           1)
			PAR_CODE(UintParameter, weightInSlot,   2)
			PAR_CODE(UintParameter, damageMin,      5)
			PAR_CODE(UintParameter, damageMax,      6)
			PAR_CODE(TimeInterval, firePeriod,      7)
			PAR_CODE(TimeInterval, shotDelay,       8)
			PAR_CODE(FloatParameter, jamProb,       9)
			PAR_CODE(FloatParameter, criticalProb,  10)
			PAR_CODE(FloatParameter, criticalCoeff, 11)

			/// Are these bools need to be grouped to one variable with bit fields?
			PAR_CODE(BoolParameter, semiAutomaticAllowed, 12) ///< Is no need in bolt disorting between shots
			PAR_CODE(BoolParameter, automaticAllowed,     13)

			// Reload cycle control
			PAR_CODE(BoolParameter, reloadIsMagazineSmart,   15) ///< Is magazine smart (with MCU)?

			PAR_CODE(BoolParameter, reloadNeedMagDisconnect, 16) ///< 1. Should the magazine be disconnected?
			PAR_CODE(BoolParameter, reloadNeedMagChange,     17) ///< 2. Should the magazine be changed?
			PAR_CODE(BoolParameter, reloadNeedBolt,          18) ///< 3. Should the bolt be distorted?

			PAR_CODE(BoolParameter, reloadPlaySound,         20) ///< Should reloading sound be played?

			PAR_CODE(UintParameter, magazinesCountStart,      30)
			PAR_CODE(UintParameter, magazinesCountMax,        31)

			PAR_CODE(UintParameter, bulletsInMagazineStart,   32)
			PAR_CODE(UintParameter, bulletsInMagazineMax,     33)

			PAR_CODE(TimeInterval, reloadingTime,            34)

			PAR_CODE(FloatParameter, heatPerShot,              40)
			PAR_CODE(FloatParameter, heatLossPerSec,           41)

			PAR_CODE(TimeInterval, fireFlashPeriod,           50)
			PAR_CODE(TimeInterval, fireVibroPeriod,           51)

			PAR_CODE(DeviceAddress, headSensorAddr,           80)

			PAR_CODE(UintParameter, outputPower,              90)
		}

		namespace State
		{
			// Rifle state
			PAR_CODE(UintParameter, bulletsInMagazineCurrent, 101)
			PAR_CODE(UintParameter, magazinesCountCurrent,    102)

			PAR_CODE(UintParameter, heatnessCurrent,    110)
		}

		namespace Functions
		{
			FUNC_CODE_NP(rifleTurnOff,       201)
			FUNC_CODE_NP(rifleTurnOn,        202)
			FUNC_CODE_NP(rifleReset,         203)
			FUNC_CODE_NP(rifleRespawn,       204)
			FUNC_CODE_NP(rifleDie,           205)
			FUNC_CODE_NP(rifleWound,         206)
			FUNC_CODE_1P(rifleShock, TimeInterval,        207)

			FUNC_CODE_1P(riflePlayEnemyDamaged, uint8_t,  210)
			FUNC_CODE_NP(headSensorToRifleHeartbeat,      220)
			FUNC_CODE_1P(rifleChangeHS, DeviceAddress,    225)
		}
	}

	namespace HeadSensor
	{
		namespace Configuration
		{
			/// Player configuration
			PAR_CODE(UintParameter, healthMax,   1000)
			PAR_CODE(UintParameter, armorMax,    1001)
			PAR_CODE(UintParameter, healthStart, 1003)
			PAR_CODE(UintParameter, armorStart,  1004)

			PAR_CODE(BoolParameter, isHealable, 1010)

			PAR_CODE(UintParameter, lifesCount,  1011)

			/// Effectivity of armor
			PAR_CODE(FloatParameter, armorCoeffStart,  1012)

			/// Simply multiplied by damage to player
			PAR_CODE(FloatParameter, damageCoeffStart, 1013)

			/// Multiplied by player's shutting damage
			PAR_CODE(FloatParameter, shotsCoeffStart,  1014)

			/// Multiplied by friendly fire
			PAR_CODE(FloatParameter, frendlyFireCoeff, 1015)

			PAR_CODE(FloatParameter, selfShotCoeff,    1016)


			PAR_CODE(TimeInterval, preRespawnDelay,    1021)
			PAR_CODE(TimeInterval, postRespawnDelay,   1022)
			PAR_CODE(BoolParameter,autoRespawn,        1023)
			PAR_CODE(TimeInterval, shockDelayImmortal, 1024)
			PAR_CODE(TimeInterval, shockDelayInactive, 1025)


			//PAR_CODE(UintParameter, plyerId,        1030)
			PAR_CODE(PlayerGameId,   plyerMT2Id,     1031)
			PAR_CODE(TeamGameId,     teamId,         1032)

			PAR_CODE(UintParameter, slot1MaxWeight,    1041)
			PAR_CODE(UintParameter, slot2MaxWeight,    1042)
			PAR_CODE(UintParameter, slot3MaxWeight,    1043)
			PAR_CODE(UintParameter, slot4MaxWeight,    1044)
			PAR_CODE(UintParameter, slot5MaxWeight,    1045)


			PAR_CODE(FloatParameter, zone1DamageCoeff,    1051)
			PAR_CODE(FloatParameter, zone2DamageCoeff,    1052)
			PAR_CODE(FloatParameter, zone3DamageCoeff,    1053)
			PAR_CODE(FloatParameter, zone4DamageCoeff,    1054)
			PAR_CODE(FloatParameter, zone5DamageCoeff,    1055)
			PAR_CODE(FloatParameter, zone6DamageCoeff,    1056)
		}

		namespace State
		{
			PAR_CODE(UintParameter, healthCurrent,      1100)
			PAR_CODE(UintParameter, armorCurrent,       1101)
			PAR_CODE(FloatParameter, armorCoeffCurrent,  1102)
			PAR_CODE(FloatParameter, damageCoeffCurrent, 1103)
			PAR_CODE(FloatParameter, shotsCoeffCurrent,  1104)

			PAR_CODE(UintParameter, lifesCountCurrent,  1005)

			PAR_CODE(UintParameter, pointsCount, 1110)
			PAR_CODE(UintParameter, killsCount,  1111)
			PAR_CODE(UintParameter, deathsCount, 1112)

			PAR_CODE(ConnectedWeaponsList, weaponsList, 1200)
		}

		namespace Functions
		{
			FUNC_CODE_NP(playerTurnOff,      1201)
			FUNC_CODE_NP(playerTurnOn,       1202)
			FUNC_CODE_NP(playerReset,        1203)
			FUNC_CODE_NP(playerRespawn,      1204)
			FUNC_CODE_NP(playerKill,         1205)

			FUNC_CODE_NP(resetStats,         1210)
			FUNC_CODE_1P(readStats,  DeviceAddress,  1211)

			FUNC_CODE_1P(addMaxHealth, IntParameter, 1220)
			FUNC_CODE_1P(setTeam,      TeamGameId,    1221)

			FUNC_CODE_1P(registerWeapon,    DeviceAddress,  1300)
			FUNC_CODE_1P(deregisterWeapon,  DeviceAddress,  1301)

			FUNC_CODE_1P(notifyIsDamager, DamageNotification, 1400)
			FUNC_CODE_1P(catchShot, ShotMessage, 1401)

			FUNC_CODE_NP(rifleToHeadSensorHeartbeat, 1500)
		}
	}

	namespace SmartPoint
	{
		namespace Configuration
		{
			PAR_CODE(UintParameter, secondsToWin,    1800)
		}
		namespace State
		{
			PAR_CODE(UintParameter, team1TimeLeft,    1851)
			PAR_CODE(UintParameter, team2TimeLeft,    1852)
			PAR_CODE(UintParameter, team3TimeLeft,    1853)
			PAR_CODE(UintParameter, team4TimeLeft,    1854)

			PAR_CODE(TeamGameId, currentTeam,    1860)
			PAR_CODE(uint8_t, gameState,    1861)

		}
	}

	namespace Base
	{
		namespace Functions
		{
			FUNC_CODE_1P(getPvPResults, GameLog::PvPDamageResults, 3001)
		}
	}
}


#endif /* LOGIC_CONFIGS_H_ */

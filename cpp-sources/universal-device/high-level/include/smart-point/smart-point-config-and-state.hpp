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

#ifndef UNIVERSAL_DEVICE_LDSCRIPTS_SMART_POINT_STATE_HPP_
#define UNIVERSAL_DEVICE_LDSCRIPTS_SMART_POINT_STATE_HPP_

#include "hal/system-clock.hpp"
#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/operation-codes.hpp"

class SmartPointConfig
{
public:
	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::Configuration, secondsToWin);
	void resetToDefault();
};

class SmartPointState
{
public:
	constexpr static uint8_t gameStateInProcess = 1;
	constexpr static uint8_t gameStateEnd = 2;

	SmartPointState(const SmartPointConfig& config);

	// This variables to be displayed or read remotely, but real calculations are in us
	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, team1TimeLeft);
	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, team2TimeLeft);
	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, team3TimeLeft);
	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, team4TimeLeft);

	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, currentTeam);
	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, gameState);

	void beginGame();
	void stopGame();
	void resetAllTime();
	void ticTime();
	void acitateByTeam(TeamGameId team);

private:
	const SmartPointConfig& m_config;
	void timeLeftUsToSec();
	void win(TeamGameId team);

	Time m_lastTimeTick = 0;

	Time m_team1TimeLeftus = 0;
	Time m_team2TimeLeftus = 0;
	Time m_team3TimeLeftus = 0;
	Time m_team4TimeLeftus = 0;
};







#endif /* UNIVERSAL_DEVICE_LDSCRIPTS_SMART_POINT_STATE_HPP_ */

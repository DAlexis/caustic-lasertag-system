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

#include "smart-point/smart-point-config-and-state.hpp"
#include "core/logging.hpp"

///////////////////////
//

SmartPointConfig::SmartPointConfig(RCSPAggregator& aggregator) :
	m_aggregator(aggregator)
{
	resetToDefault();
}

void SmartPointConfig::resetToDefault()
{
	secondsToWin = 1*60;
}

SmartPointState::SmartPointState(const SmartPointConfig& config, RCSPAggregator &aggregator) :
	m_aggregator(aggregator),
	m_config(config)
{
	gameState = gameStateEnd;
	team1TimeLeft = 0;
	team2TimeLeft = 0;
	team3TimeLeft = 0;
	team4TimeLeft = 0;
	currentTeam = 0;
}

void SmartPointState::beginGame()
{
	resetAllTime();
	gameState = gameStateInProcess;
	m_lastTimeTick = systemClock->getTime();
}

void SmartPointState::stopGame()
{
	resetAllTime();
	gameState = gameStateEnd;
}

void SmartPointState::resetAllTime()
{
	m_team1TimeLeftus = m_config.secondsToWin * 1e6;
	m_team2TimeLeftus = m_config.secondsToWin * 1e6;
	m_team3TimeLeftus = m_config.secondsToWin * 1e6;
	m_team4TimeLeftus = m_config.secondsToWin * 1e6;
	timeLeftUsToSec();

	currentTeam = teamGameIdNotATeam;
}

void SmartPointState::ticTime()
{
	if (currentTeam == teamGameIdNotATeam || gameState != gameStateInProcess)
		return;

	Time currentTime = systemClock->getTime();
	TimeInterval delta = currentTime - m_lastTimeTick;
	m_lastTimeTick = currentTime;
	switch(currentTeam)
	{
	case 0:
		if (delta >= m_team1TimeLeftus)
		{
			m_team1TimeLeftus = 0;
			win(currentTeam);
		} else {
			m_team1TimeLeftus -= delta;
		}
		break;

	case 1:
		if (delta >= m_team2TimeLeftus)
		{
			m_team2TimeLeftus = 0;
			win(currentTeam);
		} else {
			m_team2TimeLeftus -= delta;
		}
		break;

	case 2:
		if (delta >= m_team3TimeLeftus)
		{
			m_team3TimeLeftus = 0;
			win(currentTeam);
		} else {
			m_team3TimeLeftus -= delta;
		}
		break;

	case 3:
		if (delta >= m_team4TimeLeftus)
		{
			m_team4TimeLeftus = 0;
			win(currentTeam);
		} else {
			m_team4TimeLeftus -= delta;
		}
		break;
	}
	timeLeftUsToSec();
}

void SmartPointState::acitateByTeam(TeamGameId team)
{
	if (gameState == gameStateInProcess)
	{
		currentTeam = team;
	}
}

void SmartPointState::timeLeftUsToSec()
{
	team1TimeLeft = m_team1TimeLeftus / 1e6;
	team2TimeLeft = m_team2TimeLeftus / 1e6;
	team3TimeLeft = m_team3TimeLeftus / 1e6;
	team4TimeLeft = m_team4TimeLeftus / 1e6;
}

void SmartPointState::win(TeamGameId team)
{
	info << "Team " << team << " wins";
	currentTeam = team;
	gameState = gameStateEnd;
}

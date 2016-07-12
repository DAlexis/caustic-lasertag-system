/*
 * smart-point-config-and-state.cpp
 *
 *  Created on: 11 июл. 2016 г.
 *      Author: alexey
 */

#include "smart-point/smart-point-config-and-state.hpp"

///////////////////////
//
void SmartPointConfig::resetToDefault()
{
	secondsToWin = 3*60;
}

SmartPointState::SmartPointState(const SmartPointConfig& config) :
	m_config(config)
{
}

void SmartPointState::beginGame()
{
	resetAllTime();
	gameState = gameStateInProcess;
	m_lastTimeTick = systemClock->getTime();
}

void SmartPointState::resetAllTime()
{
	m_team1TimeLeftus = m_config.secondsToWin * 1e6;
	m_team2TimeLeftus = m_config.secondsToWin * 1e6;
	m_team3TimeLeftus = m_config.secondsToWin * 1e6;
	m_team4TimeLeftus = m_config.secondsToWin * 1e6;
	timeLeftUsToSec();

	currentTeam = MT2NotATeam;
}

void SmartPointState::ticTime()
{
	if (currentTeam == MT2NotATeam || gameState != gameStateInProcess)
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
			gameState = gameStateEnd;
		} else {
			m_team1TimeLeftus -= delta;
		}
		break;

	case 1:
		if (delta >= m_team2TimeLeftus)
		{
			m_team2TimeLeftus = 0;
			gameState = gameStateEnd;
		} else {
			m_team2TimeLeftus -= delta;
		}
		break;

	case 2:
		if (delta >= m_team3TimeLeftus)
		{
			m_team3TimeLeftus = 0;
			gameState = gameStateEnd;
		} else {
			m_team3TimeLeftus -= delta;
		}
		break;

	case 3:
		if (delta >= m_team4TimeLeftus)
		{
			m_team4TimeLeftus = 0;
			gameState = gameStateEnd;
		} else {
			m_team4TimeLeftus -= delta;
		}
		break;
	}
	timeLeftUsToSec();
}

void SmartPointState::acitateByTeam(TeamMT2Id team)
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

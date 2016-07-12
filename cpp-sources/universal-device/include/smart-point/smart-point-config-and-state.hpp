/*
 * smart-point-state.hpp
 *
 *  Created on: 9 июл. 2016 г.
 *      Author: dalexies
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
	void acitateByTeam(TeamMT2Id team);

private:
	const SmartPointConfig& m_config;
	void timeLeftUsToSec();
	void win(TeamMT2Id team);

	Time m_lastTimeTick = 0;

	Time m_team1TimeLeftus = 0;
	Time m_team2TimeLeftus = 0;
	Time m_team3TimeLeftus = 0;
	Time m_team4TimeLeftus = 0;
};







#endif /* UNIVERSAL_DEVICE_LDSCRIPTS_SMART_POINT_STATE_HPP_ */

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
};

class SmartPointState
{
public:
	SmartPointState(const SmartPointConfig& config);

	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, team1TimeLeft);
	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, team2TimeLeft);
	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, team3TimeLeft);
	PAR_ST(RESTORABLE, ConfigCodes::SmartPoint::State, team4TimeLeft);

	void resetAllTime();
private:
	const SmartPointConfig& m_config;
};







#endif /* UNIVERSAL_DEVICE_LDSCRIPTS_SMART_POINT_STATE_HPP_ */

/*
 * smart-point-config-and-state.cpp
 *
 *  Created on: 11 июл. 2016 г.
 *      Author: alexey
 */

#include "smart-point/smart-point-config-and-state.hpp"

SmartPointState::SmartPointState(const SmartPointConfig& config) :
	m_config(config)
{
}

void SmartPointState::resetAllTime()
{
	team1TimeLeft = m_config.secondsToWin;
	team2TimeLeft = m_config.secondsToWin;
	team3TimeLeft = m_config.secondsToWin;
	team4TimeLeft = m_config.secondsToWin;
}

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


#include "hw/system-clock-hw.hpp"
#include "utils/memory.hpp"

SystemClock clock;
ISystemClock *systemClock = &clock;

constexpr uint32_t msInSec = 1000000;
constexpr uint32_t msTimerPeriod = UINT16_MAX;
constexpr uint32_t slaveTimerPeriod = UINT16_MAX;

void SystemClock::init()
{
	MX_TIM4_Init();
	MX_TIM5_Init();
	HAL_TIM_Base_Start(&htim4);
	HAL_TIM_Base_Start(&htim5);
}

void SystemClock::MX_TIM4_Init()
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	zerify(sClockSourceConfig);
	zerify(sMasterConfig);

	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 72*0.7-1; // I have no idea why I should mul by ~0.7
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = msTimerPeriod;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim4);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);
}

void SystemClock::MX_TIM5_Init()
{
	TIM_SlaveConfigTypeDef sSlaveConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim5.Instance = TIM5;
	htim5.Init.Prescaler = 0;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim5.Init.Period = slaveTimerPeriod;
	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim5);

	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
	sSlaveConfig.InputTrigger = TIM_TS_ITR2;
	HAL_TIM_SlaveConfigSynchronization(&htim5, &sSlaveConfig);
}

Time SystemClock::getTime()
{
	uint16_t slave = TIM5->CNT;
	uint16_t us = TIM4->CNT;

	if (us < 3) {
		us = TIM4->CNT;
		slave = TIM5->CNT;
	}

	Time result = slave * msTimerPeriod + us;

	return result;
}

void SystemClock::wait_us(uint32_t time)
{
	Time lastTime = getTime();
	while (getTime()-lastTime < time) { }

}

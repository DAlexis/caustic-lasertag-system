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

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_SYSTEM_CLOCK_HPP_
#define LAZERTAG_RIFLE_INCLUDE_HW_SYSTEM_CLOCK_HPP_

#include "hal/system-clock.hpp"
#include "stm32f1xx_hal.h"

#include <stdint.h>

class SystemClock : public ISystemClock
{
public:
	void init();
	void wait_us(uint32_t time);
	Time getTime();
private:
	constexpr static uint32_t msInSec = 1000000;
	constexpr static uint32_t msTimerPeriod = UINT16_MAX;
	constexpr static uint32_t slaveTimerPeriod = UINT16_MAX;

	void MX_TIM4_Init();
	void MX_TIM5_Init();
	TIM_HandleTypeDef htim4;
	TIM_HandleTypeDef htim5;
};


#endif /* LAZERTAG_RIFLE_INCLUDE_HW_SYSTEM_CLOCK_HPP_ */

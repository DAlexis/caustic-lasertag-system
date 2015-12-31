/*
 * system-clock.hpp
 *
 *  Created on: 29 дек. 2014 г.
 *      Author: alexey
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

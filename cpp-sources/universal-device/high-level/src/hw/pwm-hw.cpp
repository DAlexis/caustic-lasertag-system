/*
 * pwm-hw.cpp
 *
 *  Created on: 4 февр. 2017 г.
 *      Author: dalexies
 */

#include "hw/pwm-hw.hpp"
#include "core/logging.hpp"
#include "utils/macro.hpp"

PWMsPool PWMsPool;

IPWMsPool* PWMs = &PWMsPool;

PWMManager::PWMManager(uint8_t timerNumber)
{
	UNUSED_ARG(timerNumber);
	MX_TIM3_Init();
	htim = &htim3;

}

void PWMManager::init(bool ch1, bool ch2, bool ch3, bool ch4)
{
	if (ch1) setChannel1(0);
	if (ch2) setChannel2(0);
	if (ch3) setChannel3(0);
	if (ch4) setChannel4(0);

	if (ch1) HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
	if (ch2) HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
	if (ch3) HAL_TIM_PWM_Start(htim, TIM_CHANNEL_3);
	if (ch4) HAL_TIM_PWM_Start(htim, TIM_CHANNEL_4);
}

void PWMManager::setChannel1(uint32_t value)
{
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, value);
}

void PWMManager::setChannel2(uint32_t value)
{
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, value);
}

void PWMManager::setChannel3(uint32_t value)
{
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, value);
}

void PWMManager::setChannel4(uint32_t value)
{
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, value);
}


IPWMManager* PWMsPool::getPWM(uint8_t timerNumber)
{
	if (timerNumber != 3)
	{
		error << "Timer " << timerNumber << " not supported for PWM Manager";
		return nullptr;
	}
	if (m_mgr == nullptr)
	{
		m_mgr = new PWMManager(timerNumber);
	}
	return m_mgr;
}

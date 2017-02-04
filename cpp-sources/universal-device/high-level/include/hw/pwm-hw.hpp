/*
 * pwm-hw.hpp
 *
 *  Created on: 4 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HW_PWM_HW_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HW_PWM_HW_HPP_

#include "hal/pwm.hpp"
#include "tim.h"

class PWMManager : public IPWMManager
{
public:
	PWMManager(uint8_t timerNumber);
	void init(bool ch1, bool ch2, bool ch3, bool ch4);
	void setChannel1(uint32_t value);
	void setChannel2(uint32_t value);
	void setChannel3(uint32_t value);
	void setChannel4(uint32_t value);

private:
	TIM_HandleTypeDef *htim;
};

class PWMsPool : public IPWMsPool
{
public:
	IPWMManager* getPWM(uint8_t timerNumber);

private:
	IPWMManager* m_mgr = nullptr;
};



#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HW_PWM_HW_HPP_ */

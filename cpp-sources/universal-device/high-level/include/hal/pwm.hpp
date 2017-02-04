/*
 * pwm.hpp
 *
 *  Created on: 1 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HAL_PWM_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HAL_PWM_HPP_

#include <stdint.h>

class IPWMManager
{
public:
	virtual ~IPWMManager() {}
	virtual void init(bool ch1, bool ch2, bool ch3, bool ch4) = 0;
	virtual void setChannel1(uint32_t value) = 0;
	virtual void setChannel2(uint32_t value) = 0;
	virtual void setChannel3(uint32_t value) = 0;
	virtual void setChannel4(uint32_t value) = 0;
};

class IPWMsPool
{
public:
	virtual ~IPWMsPool() {}
	virtual IPWMManager* getPWM(uint8_t timerNumber) = 0;
};

extern IPWMsPool* PWMs;

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HAL_PWM_HPP_ */

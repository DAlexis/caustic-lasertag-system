/*
 * adc-hw.hpp
 *
 *  Created on: 7 нояб. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_HW_ADC_HW_HPP_
#define INCLUDE_HW_ADC_HW_HPP_

#include "hal/adc.hpp"
#include "stm32f1xx_hal.h"

class ADC : public IADC
{
public:
	Result init(uint8_t port, uint8_t pin);
	UintParameter get();
	UintParameter max();
private:
	ADC_HandleTypeDef m_hadc;
};

class ADCBuilder
{
public:
	~ADCBuilder() {}
	IADC* create();
};

#endif /* INCLUDE_HW_ADC_HW_HPP_ */

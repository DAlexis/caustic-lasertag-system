/*
 * adc.hpp
 *
 *  Created on: 7 нояб. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_HAL_ADC_HPP_
#define INCLUDE_HAL_ADC_HPP_

#include "rcsp/base-types.hpp"
#include "core/result-code.hpp"

class IADC
{
public:
	virtual ~IADC() {}
	virtual Result init(uint8_t port, uint8_t pin) = 0;
	virtual UintParameter get() = 0;
	virtual UintParameter max() = 0;
};

class IADCBuilder
{
public:
	virtual ~IADCBuilder() {}
	virtual IADC* create();
};

extern IADCBuilder ADCs;

#endif /* INCLUDE_HAL_ADC_HPP_ */

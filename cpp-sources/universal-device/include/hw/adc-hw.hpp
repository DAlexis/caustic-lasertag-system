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

class ADCPool : public IADCPool
{
public:
	IADC* get() override;
};

#endif /* INCLUDE_HW_ADC_HW_HPP_ */

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


#include "hw/adc-hw.hpp"
#include "core/logging.hpp"
#include "utils/memory.hpp"

ADCBuilder builder;
IADCBuilder* ADCs = &builder;

IADC* ADCBuilder::create()
{
	return new ADC;
}

Result ADC::init(uint8_t port, uint8_t pin)
{
	/// @todo Add here code for ADC chanel selection
	ADC_ChannelConfTypeDef sConfig;
	zerify(m_hadc);
	// Common config
	m_hadc.Instance = ADC1;
	m_hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	//m_hadc.Init.ContinuousConvMode = DISABLE;
	m_hadc.Init.ContinuousConvMode = ENABLE;
	m_hadc.Init.DiscontinuousConvMode = DISABLE;
	m_hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	m_hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	m_hadc.Init.NbrOfConversion = 1;
	HAL_ADC_Init(&m_hadc);

	// Configure Regular Channel
	sConfig.Channel = ADC_CHANNEL_14;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	HAL_ADC_ConfigChannel(&m_hadc, &sConfig);
	return Result();
}

UintParameter ADC::get()
{
	uint32_t result = 0;
	HAL_ADC_Start(&m_hadc);
	HAL_StatusTypeDef status = HAL_ADC_PollForConversion(&m_hadc, 5000);
	if (status != HAL_OK)
		error << "HAL_ADC_PollForConversion returned " << status;
	result = HAL_ADC_GetValue(&m_hadc);
	HAL_ADC_Stop(&m_hadc);
	return result;
}

UintParameter ADC::max()
{
	return (1<<12) - 1;
}

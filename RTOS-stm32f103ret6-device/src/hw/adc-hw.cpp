/*
 * adc.cpp
 *
 *  Created on: 7 нояб. 2015 г.
 *      Author: alexey
 */

#include "hw/adc-hw.hpp"

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

	// Common config
	m_hadc.Instance = ADC1;
	m_hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	m_hadc.Init.ContinuousConvMode = DISABLE;
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
	HAL_ADC_PollForConversion(&m_hadc, 100);
	result = HAL_ADC_GetValue(&m_hadc);
	HAL_ADC_Stop(&m_hadc);
	return result;
}

UintParameter ADC::max()
{
	return 0b111111111111;
}

/*
 * power-minitor.cpp
 *
 *  Created on: 26 марта 2016 г.
 *      Author: alexey
 */

#include "core/power-monitor.hpp"
#include "hal/adc.hpp"
#include "core/logging.hpp"

SINGLETON_IN_CPP(PowerMonitor)

PowerMonitor::PowerMonitor()
{
	m_adc = ADCs->create();
	m_adc->init(0,0);
	supplyVoltageMin = 3.7;
	supplyVoltageMax = 4.2;
	adcCalibrationCoeff = 2.0;
}

void PowerMonitor::interrogate()
{
	UintParameter value = m_adc->get();

	supplyVoltage = ((float)value) * 3.3 * adcCalibrationCoeff * m_internalCalibrationCoeff / m_adc->max();

	if (supplyVoltage > supplyVoltageMin)
	{
		chargePercent = (supplyVoltage - supplyVoltageMin) / (supplyVoltageMax - supplyVoltageMin) * 100;
		if (chargePercent > 100)
			chargePercent = 100;
	} else
		chargePercent = 0;
}

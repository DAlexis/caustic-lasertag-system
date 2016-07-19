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

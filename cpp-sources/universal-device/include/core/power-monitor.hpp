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

#ifndef INCLUDE_CORE_POWER_MONITOR_HPP_
#define INCLUDE_CORE_POWER_MONITOR_HPP_

#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/operation-codes.hpp"
#include "utils/macro.hpp"
#include "hal/adc.hpp"

#include "core/os-wrappers.hpp"

class PowerMonitor : public IInterrogatable
{
public:
	PAR_ST(NOT_RESTORABLE, ConfigCodes::AnyDevice::State, supplyVoltage);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, supplyVoltageMin);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, supplyVoltageMax);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, adcCalibrationCoeff);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::AnyDevice::State, chargePercent);

	void interrogate() override;

	SIGLETON_IN_CLASS(PowerMonitor)

private:
	PowerMonitor();

	IADC *m_adc;
	float m_internalCalibrationCoeff = 1.1954;
};







#endif /* INCLUDE_CORE_POWER_MONITOR_HPP_ */

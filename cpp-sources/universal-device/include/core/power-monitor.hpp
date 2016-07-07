/*
 * power-monitor.hpp
 *
 *  Created on: 26 марта 2016 г.
 *      Author: alexey
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

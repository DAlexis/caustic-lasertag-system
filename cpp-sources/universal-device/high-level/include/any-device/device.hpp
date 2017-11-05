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


#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_

#include <any-device/device-base-types.hpp>
#include "rcsp/aggregator.hpp"
#include "rcsp/operation-codes.hpp"
#include "rcsp/RCSP-base-types.hpp"
#include <stdint.h>
#include <string.h>

class DeviceConfiguration
{
public:
	DeviceConfiguration(RCSPAggregator &aggregator);

private:
	RCSPAggregator &m_aggregator;

public:

	PAR_CL(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, devAddr, m_aggregator);
	PAR_CL(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, deviceName, m_aggregator);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, deviceType, m_aggregator);
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_ */

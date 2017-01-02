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


#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_MODEM_BASE_TYPES_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_MODEM_BASE_TYPES_HPP_

#include <stdint.h>
#include <functional>

using PackageId = uint16_t;

struct DeviceAddress
{
	constexpr static uint8_t size = 3;
	uint8_t address[size];

	DeviceAddress(uint8_t a0 = 1, uint8_t a1 = 1, uint8_t a2 = 1)
		{ address[0] = a0; address[1] = a1, address[2] = a2; }

	void convertFromString(const char* str);

	// Operators
	inline bool operator==(const DeviceAddress& other) const
	{
		for(int i=0; i<size; i++)
			if (address[i] != other.address[i])
				return false;
		return true;
	}

	inline bool operator!=(const DeviceAddress& other) const
	{
		return not (*this == other);
	}

	inline bool operator<(const DeviceAddress& other) const
	{
		for(int i=0; i<size; i++)
		{
			if (address[i] < other.address[i])
				return true;
			if (address[i] > other.address[i])
				return false;
		}
		return false;
	}
};

struct PackageTimings
{
	constexpr static uint32_t defaultTimeout = 10000000;
	constexpr static uint32_t defaultResendTime = 200000;
	constexpr static uint32_t defaultResendTimeDelta = 100000;

	PackageTimings(
			bool _infiniteResend = false,
			uint32_t _timeout = defaultTimeout,
			uint32_t _resendTime = defaultResendTime,
			uint32_t _resendTimeDelta = defaultResendTimeDelta
			) : infiniteResend(_infiniteResend), timeout(_timeout), resendTime(_resendTime), resendTimeDelta(_resendTimeDelta)
	{ }

	bool infiniteResend = false;
	uint32_t timeout = defaultTimeout;
	uint32_t resendTime = defaultResendTime;
	uint32_t resendTimeDelta = defaultResendTimeDelta;
};

using ReceivePackageCallback = std::function<void(DeviceAddress /*sender*/, uint8_t* /*payload*/, uint16_t /*payload length*/)>;
using PackageSendingDoneCallback = std::function<void(PackageId /*package_id*/, bool /*was successfully sent*/)>;

#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_MODEM_BASE_TYPES_HPP_ */

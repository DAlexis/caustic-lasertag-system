/*
 * modem-base-types.hpp
 *
 *  Created on: 8 сент. 2015 г.
 *      Author: alexey
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
	constexpr static uint32_t defaultTimeout = 20000000;
	constexpr static uint32_t defaultResendTime = 500000;
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

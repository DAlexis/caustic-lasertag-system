/*
 * rcsp-base-types.hpp
 *
 *  Created on: 4 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_RCSP_BASE_TYPES_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_RCSP_BASE_TYPES_HPP_

#include <stdint.h>

#define ADDRESS_TO_STREAM(addr)      (addr).address[0] << "-" << (addr).address[1] << "-" << (addr).address[2]

using OperationSize = uint8_t;
using OperationCode = uint16_t;

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


#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_RCSP_RCSP_BASE_TYPES_HPP_ */

/*
 * device-base-types.hpp
 *
 *  Created on: 13 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_DEVICE_BASE_TYPES_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_DEVICE_BASE_TYPES_HPP_

#include <stdint.h>

struct DeviceName
{
	constexpr static uint8_t nameLength = 20;
	char name[20] = "Unknown device";

	void convertFromString(const char* str);
};

namespace DeviceTypes
{
	constexpr uint16_t rifle = 1;
	constexpr uint16_t headSensor = 2;
	constexpr uint16_t bluetoothBridge = 3;
	constexpr uint16_t smartPoint = 4;

};
#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_DEVICE_DEVICE_BASE_TYPES_HPP_ */

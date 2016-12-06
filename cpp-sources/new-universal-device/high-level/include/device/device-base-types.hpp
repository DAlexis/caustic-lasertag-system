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

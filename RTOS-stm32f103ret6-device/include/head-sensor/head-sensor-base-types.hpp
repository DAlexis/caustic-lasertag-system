/*
 * head-sensor-base-types.hpp
 *
 *  Created on: 4 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HEAD_SENSOR_HEAD_SENSOR_BASE_TYPES_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HEAD_SENSOR_HEAD_SENSOR_BASE_TYPES_HPP_

#include "rcsp/RCSP-base-types.hpp"
#include "communication/head-sensor-rifle-communication.hpp"
#include <set>

class ConnectedWeaponsList
{
public:
	void deserialize(void* source, OperationSize size);
	void serialize(void* destination);
	uint32_t serializedSize();

	std::set<DeviceAddress> weapons;
};



#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HEAD_SENSOR_HEAD_SENSOR_BASE_TYPES_HPP_ */

/*
 * device.hpp
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_

#include "logic/configuration.hpp"
#include <stdint.h>

struct DeviceAddress
{
	uint8_t address[3];
};

class DeviceParameters
{
public:
	PARAMETER(ConfigCodes::Device, DeviceAddress, devAddr);
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_ */

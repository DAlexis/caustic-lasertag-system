/*
 * device.hpp
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_

#include "logic/config-codes.hpp"
#include <stdint.h>
#include <string.h>

#define ADDRESS_LENGTH          3

struct DeviceAddress
{
	uint8_t address[3];
};

class DeviceParameters
{
public:
	DeviceParameters() :
		devAddr({1, 1, 1})
	{ }
	PARAMETER(ConfigCodes::Device, DeviceAddress, devAddr);
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_ */

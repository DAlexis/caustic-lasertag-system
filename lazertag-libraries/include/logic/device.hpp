/*
 * device.hpp
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_

#include "rcsp/operation-codes.hpp"
#include "rcsp/RCSP-modem.hpp"
#include <stdint.h>
#include <string.h>

class DeviceParameters
{
public:
	DeviceParameters() :
		devAddr({1, 1, 1})
	{ }
	PARAMETER(ConfigCodes::Device, DeviceAddress, devAddr);
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_ */

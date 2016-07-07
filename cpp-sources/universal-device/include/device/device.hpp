/*
 * device.hpp
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_

#include "device/device-base-types.hpp"
#include "rcsp/operation-codes.hpp"
#include "rcsp/RCSP-base-types.hpp"
#include "rcsp/RCSP-aggregator.hpp"
#include <stdint.h>
#include <string.h>

class DeviceConfiguration
{
public:

	PAR_CL(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, devAddr);
	PAR_CL(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, deviceName);
	PAR_ST(NOT_RESTORABLE, ConfigCodes::AnyDevice::Configuration, deviceType);
};

#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_DEVICE_HPP_ */

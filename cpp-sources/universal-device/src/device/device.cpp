/*
 * device.cpp
 *
 *  Created on: 13 сент. 2015 г.
 *      Author: alexey
 */

#include "device/device.hpp"
#include <string.h>

void DeviceName::convertFromString(const char* str)
{
	strlcpy(name, str, nameLength);
}

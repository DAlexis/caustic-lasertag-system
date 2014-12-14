/*
 * abstract-devices-pool.hpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_HAL_ABSTRACT_DEVICES_POOL_HPP_
#define LAZERTAG_DEVICE_INCLUDE_HAL_ABSTRACT_DEVICES_POOL_HPP_

#include "miles-tag-2.hpp"

class IAbstractDevicesPool
{
public:
	virtual ~IAbstractDevicesPool() {}
	virtual IMilesTag2Transmitter* getMilesTagTransmitter() = 0;
	virtual IMilesTag2Receiver* getMilesTagReceiver() = 0;
private:
};

extern IAbstractDevicesPool* devicesPool;


#endif /* LAZERTAG_DEVICE_INCLUDE_HAL_ABSTRACT_DEVICES_POOL_HPP_ */

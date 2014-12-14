/*
 * devices-pool.hpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_HARDWARE_DEPENDENT_DEVICES_POOL_HPP_
#define LAZERTAG_DEVICE_INCLUDE_HARDWARE_DEPENDENT_DEVICES_POOL_HPP_

#include "hal/abstract-devices-pool.hpp"

class DevicesPool : public IAbstractDevicesPool
{
public:
	DevicesPool();
	IMilesTag2Transmitter* getMilesTagTransmitter();
	IMilesTag2Receiver* getMilesTagReceiver();

private:
};

#endif /* LAZERTAG_DEVICE_INCLUDE_HARDWARE_DEPENDENT_DEVICES_POOL_HPP_ */

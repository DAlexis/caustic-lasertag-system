/*
 * devices-pool.cpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#include "hardware-dependent/devices-pool.hpp"
#include "hardware-dependent/miles-tag-2-hw.hpp"

IAbstractDevicesPool* devicesPool;
DevicesPool pool;

MilesTag2Receiver MT2Receiver;
MilesTag2Transmitter MT2Transmitter;


DevicesPool::DevicesPool()
{
	devicesPool = this;
}

IMilesTag2Transmitter* DevicesPool::getMilesTagTransmitter()
{
	return &MT2Transmitter;
}

IMilesTag2Receiver* DevicesPool::getMilesTagReceiver()
{
	return &MT2Receiver;
}

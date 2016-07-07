/*
 * head-sensor-base-types.hpp
 *
 *  Created on: 4 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HEAD_SENSOR_HEAD_SENSOR_BASE_TYPES_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HEAD_SENSOR_HEAD_SENSOR_BASE_TYPES_HPP_

#include "rcsp/RCSP-base-types.hpp"
#include "network/network-base-types.hpp"
#include "communication/head-sensor-rifle-communication.hpp"
#include <map>

class IWeaponObresver
{
public:
	virtual ~IWeaponObresver() {}
	virtual void assign(const DeviceAddress& addr) = 0;
};

class IWeaponObserverFactory
{
public:
	virtual ~IWeaponObserverFactory() {}
	virtual IWeaponObresver *create() const = 0;
};

class ConnectedWeaponsList
{
public:
	void deserialize(void* source, OperationSize size);
	void serialize(void* destination);
	uint32_t serializedSize();

	void setWeaponObserverFactory(const IWeaponObserverFactory* factory);

	void clear();
	void insert(DeviceAddress addr);
	void remove(DeviceAddress addr);

	bool empty();

	const std::map<DeviceAddress, IWeaponObresver*>& weapons();

private:
	const IWeaponObserverFactory *m_factory = nullptr;
	std::map<DeviceAddress, IWeaponObresver*> m_weapons;
};



#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HEAD_SENSOR_HEAD_SENSOR_BASE_TYPES_HPP_ */

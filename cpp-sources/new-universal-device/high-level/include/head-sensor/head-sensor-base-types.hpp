/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
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

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

#include "head-sensor/head-sensor-base-types.hpp"
#include "core/logging.hpp"
#include "rcsp/RCSP-aggregator.hpp"

//////////////////////
// ConnectedWeaponsList
void ConnectedWeaponsList::deserialize(void* source, OperationSize size)
{
	trace << "Weapons list deserializing\n";
	void* cursor = source;
	uint8_t newSize = 0;
	deserializeAndInc(cursor, newSize);
	if (size != sizeof(newSize) + newSize*sizeof(DeviceAddress))
	{
		error << "Deserializing weapons list: invalid data size\n";
		return;
	}
	m_weapons.clear();
	for (uint8_t i=0; i<newSize; i++)
	{
		DeviceAddress addr;
		deserializeAndInc(cursor, addr);
		info << "Weapon restored: " << ADDRESS_TO_STREAM(addr);
		insert(addr);
	}
}

void ConnectedWeaponsList::serialize(void* destination)
{
	trace << "Weapons list serializing\n";
	void* cursor = destination;
	uint8_t size = m_weapons.size();

	serializeAndInc(cursor, size);

	for (auto it=m_weapons.begin(); it != m_weapons.end(); it++)
	{
		serializeAndInc(cursor, it->first);
	}
}

uint32_t ConnectedWeaponsList::serializedSize()
{
	return sizeof(uint8_t) + m_weapons.size() * sizeof(DeviceAddress);
}

void ConnectedWeaponsList::setWeaponObserverFactory(const IWeaponObserverFactory* factory)
{
	m_factory = factory;
}

void ConnectedWeaponsList::clear()
{
	m_weapons.clear();
}

void ConnectedWeaponsList::insert(DeviceAddress addr)
{
	IWeaponObserver *obs;
	if (m_factory)
	{
		obs = m_factory->create();
		obs->assign(addr);
	} else {
		error << "IWeaponObresverFactory not set for ConnectedWeaponsList";
		obs = nullptr;
	}
	m_weapons[addr] = obs;
}

void ConnectedWeaponsList::remove(DeviceAddress addr)
{
	auto it = m_weapons.find(addr);
	if (it != m_weapons.end())
	{
		delete it->second;
		m_weapons.erase(it);
	}
}

const std::map<DeviceAddress, IWeaponObserver*>& ConnectedWeaponsList::weapons()
{
	return m_weapons;
}


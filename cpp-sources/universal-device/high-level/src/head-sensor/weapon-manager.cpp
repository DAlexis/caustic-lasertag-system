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


#include "head-sensor/weapon-manager.hpp"

void WeaponsManager2::updateWeapon(DeviceAddress addr)
{
	Time now = systemClock->getTime();
	auto it = m_weapons.find(addr);

	if (it == m_weapons.end())
	{
		WeaponRecord newRec;
		newRec.lastHeartbeat = now;
		m_weapons[addr] = newRec;
	} else {
		it->second.lastHeartbeat = now;
	}
}

void WeaponsManager2::applyToAny(WeaponVisitor visitor) const
{
	for (auto it=m_weapons.begin(); it!=m_weapons.end(); ++it)
	{
		visitor(it->first);
	}
}

void WeaponsManager2::applyToOne(WeaponVisitor visitor) const
{
	if (!m_weapons.empty())
		visitor(m_weapons.begin()->first);
}

void WeaponsManager2::interrogate()
{
	Time now = systemClock->getTime();
	for (auto it=m_weapons.begin(); it!=m_weapons.end(); ++it)
	{
		if (now - it->second.lastHeartbeat > timeout)
		{
			m_weapons.erase(it);
			break;
		}
	}
}

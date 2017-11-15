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

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_WEAPON_MANAGER_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_WEAPON_MANAGER_HPP_

#include "network/network-base-types.hpp"
#include "hal/system-clock.hpp"
#include "utils/interfaces.hpp"

#include <map>

#include <functional>

class WeaponsManager : public IInterrogatable
{
public:
	using WeaponVisitor = std::function<void(DeviceAddress)>;
	constexpr static uint32_t timeout = 2000000;

	/**
	 * Update last seen time for weapon by address
	 */
	void updateWeapon(DeviceAddress addr);
	void applyToAny(WeaponVisitor visitor) const;
	void applyToOne(WeaponVisitor visitor) const;

	void interrogate() override;
private:
	struct WeaponRecord
	{
		Time lastHeartbeat = 0;
	};

	std::map<DeviceAddress, WeaponRecord> m_weapons;
};






#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_WEAPON_MANAGER_HPP_ */

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

#ifndef INCLUDE_LOGIC_HEAD_SENSOR_RIFLE_COMMUNICATION_HPP_
#define INCLUDE_LOGIC_HEAD_SENSOR_RIFLE_COMMUNICATION_HPP_

#include "ir/MT2-base-types.hpp"

struct DamageNotification
{
	constexpr static uint8_t injured = 0;
	constexpr static uint8_t killed = 1;

	PlayerMT2Id damager = 0;
	uint8_t damagedTeam = 0;
	PlayerMT2Id target = 0;
	uint8_t state = 0;
};

struct NotificationSoundCase
{
	constexpr static uint8_t enemyInjured = 0;
	constexpr static uint8_t enemyKilled = 1;
	constexpr static uint8_t friendInjured = 2;

};



#endif /* INCLUDE_LOGIC_HEAD_SENSOR_RIFLE_COMMUNICATION_HPP_ */

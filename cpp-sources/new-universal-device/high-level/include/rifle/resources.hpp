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

#ifndef INCLUDE_RIFLE_RESOURCES_HPP_
#define INCLUDE_RIFLE_RESOURCES_HPP_

#include "network/network-base-types.hpp"

class RiflePackageTimings
{
public:
	PackageTimings deregistration{false, 20000000, 500000, 500000};
	PackageTimings registration{true};
};

extern RiflePackageTimings riflePackageTimings;

struct RiflePinoutMapping
{
	// Default (rifle-rev.1)
	uint8_t fireButtonPort   =   0;
	uint8_t fireButtonPin    =   0;

	uint8_t reloadButtonPort =   1;
	uint8_t reloadButtonPin  =   2;

	uint8_t automaticButtonPort      =   2;
	uint8_t automaticButtonPin       =   7;

	uint8_t semiAutomaticButtonPort  =   1;
	uint8_t semiAutomaticButtonPin   =   9;

	uint8_t magazine1SensorPort = 0;
	uint8_t magazine1SensorPin  = 11;

	uint8_t magazine2SensorPort = 0;
	uint8_t magazine2SensorPin  = 12;

	bool enableFlash = true;
	uint8_t flashPort = 1;
	uint8_t flashPin  = 11;

	bool enableVibro = true;
	uint8_t vibroPort = 0;
	uint8_t vibroPin  = 1;

	uint8_t fireChannel = 3;
};

#endif /* INCLUDE_RIFLE_RESOURCES_HPP_ */

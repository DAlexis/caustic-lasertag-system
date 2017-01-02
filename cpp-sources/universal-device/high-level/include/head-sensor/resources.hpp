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

#ifndef INCLUDE_HEAD_SENSOR_RESOURCES_HPP_
#define INCLUDE_HEAD_SENSOR_RESOURCES_HPP_

#include "network/network-base-types.hpp"
#include "dev/rgb-leds.hpp"

class HeadSensorPackageTimings
{
public:
	PackageTimings killPlayer {false, 20000000, 100000, 100000};
	PackageTimings woundPlayer{false, 20000000, 100000, 100000};
	PackageTimings damagerNotificationBroadcast{false, 1000000, 100000, 100000};
};

class BlinkPatterns
{
public:
	const RGBLeds::BlinkPattern init{100000, 100000, 2};
	const RGBLeds::BlinkPattern anyCommand{100000, 100000, 1};
	const RGBLeds::BlinkPattern death{100000, 100000, 20};
	const RGBLeds::BlinkPattern wound{50000, 50000, 3};
	const RGBLeds::BlinkPattern respawn{100000, 100000, 2};
	const RGBLeds::BlinkPattern respawnLimitIsOver{3000000, 100000, 1};
};

struct HeadSensorPinoutMapping
{
	/// Default values for head sensor rev1.1
	bool zone1Enabled = true;
	uint8_t zone1Port = 0;
	uint8_t zone1Pin  = 0;
	bool zone1VibroEnabled = true;
	uint8_t zone1VibroPort = 2;
	uint8_t zone1VibroPin  = 0;


	bool zone2Enabled = true;
	uint8_t zone2Port = 0;
	uint8_t zone2Pin  = 1;
	bool zone2VibroEnabled = true;
	uint8_t zone2VibroPort = 2;
	uint8_t zone2VibroPin  = 1;

	bool zone3Enabled = true;
	uint8_t zone3Port = 0;
	uint8_t zone3Pin  = 2;
	bool zone3VibroEnabled = true;
	uint8_t zone3VibroPort = 2;
	uint8_t zone3VibroPin  = 2;

	bool zone4Enabled = true;
	uint8_t zone4Port = 0;
	uint8_t zone4Pin  = 3;
	bool zone4VibroEnabled = true;
	uint8_t zone4VibroPort = 2;
	uint8_t zone4VibroPin  = 3;

	bool zone5Enabled = true;
	uint8_t zone5Port = 0;
	uint8_t zone5Pin  = 4;
	bool zone5VibroEnabled = true;
	uint8_t zone5VibroPort = 2;
	uint8_t zone5VibroPin  = 4;

	bool zone6Enabled = true;
	uint8_t zone6Port = 0;
	uint8_t zone6Pin  = 5;
	bool zone6VibroEnabled = true;
	uint8_t zone6VibroPort = 2;
	uint8_t zone6VibroPin  = 5;

	bool ledsEnabled = true;
	uint8_t redPort   = 1;
	uint8_t redPin    = 0;
	uint8_t greenPort = 0;
	uint8_t greenPin  = 7;
	uint8_t bluePort = 0;
	uint8_t bluePin = 6;
};

extern HeadSensorPackageTimings headSensorPackageTimings;
extern BlinkPatterns blinkPatterns;

#endif /* INCLUDE_HEAD_SENSOR_RESOURCES_HPP_ */

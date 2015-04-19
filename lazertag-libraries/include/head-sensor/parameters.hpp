/*
 * package-timings.hpp
 *
 *  Created on: 17 апр. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_HEAD_SENSOR_PARAMETERS_HPP_
#define INCLUDE_HEAD_SENSOR_PARAMETERS_HPP_

#include "rcsp/RCSP-modem.hpp"
#include "dev/rgb-leds.hpp"

class HeadSensorPackageTimings
{
public:
	PackageTimings killPlayer{20000000, 100000, 100000};
};

class BlinkPatterns
{
public:
	const RGBLeds::BlinkPattern init{100000, 100000, 2};
	const RGBLeds::BlinkPattern setTeam{100000, 100000, 2};
	const RGBLeds::BlinkPattern death{100000, 100000, 2};
	const RGBLeds::BlinkPattern wound{50000, 50000, 3};
	const RGBLeds::BlinkPattern respawn{100000, 100000, 2};
	const RGBLeds::BlinkPattern healthChange{100000, 100000, 2};
};

extern HeadSensorPackageTimings headSensorPackageTimings;
extern BlinkPatterns blinkPatterns;

#endif /* INCLUDE_HEAD_SENSOR_PARAMETERS_HPP_ */

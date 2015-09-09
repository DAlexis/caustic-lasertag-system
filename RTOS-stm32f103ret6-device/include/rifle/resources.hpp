/*
 * packages-timings.hpp
 *
 *  Created on: 16 апр. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_RIFLE_RESOURCES_HPP_
#define INCLUDE_RIFLE_RESOURCES_HPP_

#include "network/network-base-types.hpp"

class RiflePackageTimings
{
public:
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

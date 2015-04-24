/*
 * packages-timings.hpp
 *
 *  Created on: 16 апр. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_RIFLE_PACKAGES_TIMINGS_HPP_
#define INCLUDE_RIFLE_PACKAGES_TIMINGS_HPP_

#include "rcsp/RCSP-modem.hpp"

class RiflePackageTimings
{
public:
	PackageTimings registration{true};
};

extern RiflePackageTimings riflePackageTimings;

#endif /* INCLUDE_RIFLE_PACKAGES_TIMINGS_HPP_ */

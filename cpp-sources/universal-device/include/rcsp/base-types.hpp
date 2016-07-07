/*
 * base-types.hpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_RCSP_BASE_TYPES_HPP_
#define INCLUDE_RCSP_BASE_TYPES_HPP_

#include <stdint.h>

using UintParameter = uint16_t;
using IntParameter = int16_t;
using TimeInterval = uint32_t;
using FloatParameter = float;
using BoolParameter = bool;

struct RTCTime
{
	RTCTime(uint8_t h = 0, uint8_t m = 0, uint8_t s = 0) :
		hours(h),
		mins(m),
		secs(s)
	{}
	uint8_t hours;
	uint8_t mins;
	uint8_t secs;
};

struct RTCDate
{
	RTCDate(uint8_t d = 0, uint8_t m = 0, uint8_t y = 0) :
		day(d),
		month(m),
		year(y)
	{}
	uint8_t day;
	uint8_t month;
	uint8_t year;
};

struct RTCDateTime
{
	RTCDate date;
	RTCTime time;
};

#endif /* INCLUDE_RCSP_BASE_TYPES_HPP_ */

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

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


#ifndef INCLUDE_HAL_RTC_HPP_
#define INCLUDE_HAL_RTC_HPP_

#include "rcsp/base-types.hpp"

class IRTC
{
public:
	virtual ~IRTC() { }
	virtual void init() = 0;
	virtual void getDate(RTCDate& date) = 0;
	virtual void setDate(const RTCDate& date) = 0;

	virtual void getTime(RTCTime& time) = 0;
	virtual void setTime(const RTCTime& time) = 0;

	virtual void getDateTime(RTCDateTime& dateTime) = 0;
	virtual void setDateTime(const RTCDateTime& dateTime) = 0;

};

class RTCBase : public IRTC
{
public:
	void getDateTime(RTCDateTime& dateTime) { getTime(dateTime.time); getDate(dateTime.date); }
	void setDateTime(const RTCDateTime& dateTime) { setTime(dateTime.time); setDate(dateTime.date); }
};

extern IRTC* RTCManager;

#endif /* INCLUDE_HAL_RTC_HPP_ */

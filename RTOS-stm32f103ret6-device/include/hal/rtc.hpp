/*
 * rtc.hpp
 *
 *  Created on: 7 нояб. 2015 г.
 *      Author: alexey
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

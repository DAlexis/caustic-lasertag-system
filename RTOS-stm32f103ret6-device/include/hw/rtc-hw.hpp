/*
 * rtc-hw.hpp
 *
 *  Created on: 7 нояб. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_HW_RTC_HW_HPP_
#define INCLUDE_HW_RTC_HW_HPP_

#include "hal/rtc.hpp"

#include "stm32f1xx_hal.h"

class RTCMgr : public RTCBase
{
public:
	void init();
	void getDate(RTCDate& date);
	void setDate(const RTCDate& date);

	void getTime(RTCTime& time);
	void setTime(const RTCTime& time);
private:
	RTC_HandleTypeDef m_hrtc;
	static void timeToHalTime(const RTCTime& time, RTC_TimeTypeDef& halTime);
	static void halTimeToTime(RTCTime& time, const RTC_TimeTypeDef& halTime);
	static void dateToHalDate(const RTCDate& date, RTC_DateTypeDef& halDate);
	static void halDateToDate(RTCDate& date, const RTC_DateTypeDef& halDate);
};



#endif /* INCLUDE_HW_RTC_HW_HPP_ */

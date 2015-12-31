/*
 * rtc.cpp
 *
 *  Created on: 7 нояб. 2015 г.
 *      Author: alexey
 */

#include "hw/rtc-hw.hpp"
#include "core/logging.hpp"
#include "utils/memory.hpp"

RTCMgr rtcmgr;
IRTC* RTCManager = &rtcmgr;

void RTCMgr::init()
{
	/**Initialize RTC and set the Time and Date
	*/
	zerify(m_hrtc);
	m_hrtc.Instance = RTC;
	m_hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	m_hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
	m_hrtc.DateToUpdate.WeekDay = RTC_WEEKDAY_TUESDAY;
	m_hrtc.DateToUpdate.Month = RTC_MONTH_FEBRUARY;
	m_hrtc.DateToUpdate.Date = 1;
	m_hrtc.DateToUpdate.Year = 15;

	if (!isTurnedOn())
	{
		info << "RTC disabled, enabling and resetting";
		HAL_RTC_Init(&m_hrtc);
		RTCTime t(0, 0, 0);
		RTCDate d(1, 2, 3);
		setTime(t);
		setDate(d);
	}

	//if(!(*(volatile uint32_t *) (BDCR_RTCEN_BB)))__HAL_RCC_RTC_ENABLE();
}

void RTCMgr::getDate(RTCDate& date)
{
	RTC_DateTypeDef halDate;
	HAL_RTC_GetDate(&m_hrtc, &halDate, FORMAT_BIN);
	halDateToDate(date, halDate);
}

void RTCMgr::getTime(RTCTime& time)
{
	RTC_TimeTypeDef halTime;
	HAL_RTC_GetTime(&m_hrtc, &halTime, FORMAT_BIN);
	halTimeToTime(time, halTime);
}

void RTCMgr::setDate(const RTCDate& date)
{
	RTC_DateTypeDef halDate;
	dateToHalDate(date, halDate);
	HAL_RTC_SetDate(&m_hrtc, &halDate, FORMAT_BIN);
}

void RTCMgr::setTime(const RTCTime& time)
{
	RTC_TimeTypeDef halTime;
	timeToHalTime(time, halTime);
	HAL_RTC_SetTime(&m_hrtc, &halTime, FORMAT_BIN);
}

void RTCMgr::timeToHalTime(const RTCTime& time, RTC_TimeTypeDef& halTime)
{
	halTime.Hours = time.hours;
	halTime.Minutes = time.mins;
	halTime.Seconds = time.secs;
}

void RTCMgr::halTimeToTime(RTCTime& time, const RTC_TimeTypeDef& halTime)
{
	time.hours = halTime.Hours;
	time.mins = halTime.Minutes;
	time.secs = halTime.Seconds;
}

void RTCMgr::dateToHalDate(const RTCDate& date, RTC_DateTypeDef& halDate)
{
	halDate.Year = date.year;
	halDate.Month = date.month;
	halDate.Date = date.day;
}

void RTCMgr::halDateToDate(RTCDate& date, const RTC_DateTypeDef& halDate)
{
	date.year = halDate.Year;
	date.month = halDate.Month;
	date.day = halDate.Date;
}

bool RTCMgr::isTurnedOn()
{
	// Not sure this will work
	return (RCC->BDCR & RCC_BDCR_RTCEN) == RCC_BDCR_RTCEN;
}

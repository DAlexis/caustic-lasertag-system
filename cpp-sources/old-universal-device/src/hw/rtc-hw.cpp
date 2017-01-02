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

	/*
	m_hrtc.Instance = RTC;
	m_hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;

	if (HAL_RTC_Init(&m_hrtc) != HAL_OK)
	{
		// Initialization Error
		error << "Error on HAL_RTC_Init";
	}

	//##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#
	// Read the Back Up Register 1 Data
	if (HAL_RTCEx_BKUPRead(&m_hrtc, RTC_BKP_DR1) != 0x32F2)
	{
	// Configure RTC Calendar
		RTC_CalendarConfig();
	}
	else
	{
		// Check if the Power On Reset flag is set
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
		{
		  // Power on reset occured
		}
		// Check if Pin Reset flag is set
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
		{
		  // External reset occured
		}
		// Clear source Reset Flag
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}
*/
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

void RTCMgr::RTC_CalendarConfig()
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = 0x14;
  sdatestructure.Month = RTC_MONTH_FEBRUARY;
  sdatestructure.Date = 0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;

  if(HAL_RTC_SetDate(&m_hrtc,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
	  error << "Error on HAL_RTC_SetDate";
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0x02;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;

  if (HAL_RTC_SetTime(&m_hrtc, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
	  error << "Error on HAL_RTC_SetTime";
  }

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&m_hrtc, RTC_BKP_DR1, 0x32F2);
}

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

#ifndef INCLUDE_CORE_LOGGING_HPP_
#define INCLUDE_CORE_LOGGING_HPP_

#include "hal/uart.hpp"
#include <string.h>
#include <list>
#include <unistd.h>

extern "C" int _write(int file, char *ptr, int len);

class Loggers
{
	friend class Logger;
	friend class ScopedTag;
public:
	static void initLoggers();
	static bool isInitialized();

private:
	static void infoOnOff(const char* arg);
	static void debugOnOff(const char* arg);
	static void radioOnOff(const char* arg);
	static void traceOnOff(const char* arg);

	static std::list<const char*> tags;
	static bool m_isInitialized;
	static char* m_tmpBuffer;
	static uint16_t m_tmpBufferCurrent;
	constexpr static uint16_t tmpBufferSize = 1000;
};

class Logger
{
private:
	class LoggerUnnamed;

public:
	Logger(const char* loggerName) : m_loggerName(loggerName) {}

	void enable(bool enabled = true);
	bool isEnabled();

	template<class T>
	LoggerUnnamed& operator<<(T&& data)
	{
		if (m_enabled)
		{
			writePrefix();
		}

		return m_unnamedLogger << data;
	}

private:
	void writePrefix();
	struct LoggerUnnamed
	{
		constexpr static unsigned int numbersBufferSize = 30;
		LoggerUnnamed& operator<<(const char* str);
		LoggerUnnamed& operator<<(const std::string& str);
		LoggerUnnamed& operator<<(int d);
		LoggerUnnamed& operator<<(unsigned int d);

#if __arm__
		LoggerUnnamed& operator<<(uint32_t d);
#endif
		LoggerUnnamed& operator<<(uint64_t d);
		LoggerUnnamed& operator<<(float f);
		LoggerUnnamed& operator<<(double f);
		LoggerUnnamed& operator<<(bool b);
		bool enabled = false;
	};

	LoggerUnnamed m_unnamedLogger;
	const char* m_loggerName;
	bool m_enabled = false;
};

extern Logger error;
extern Logger warning;
extern Logger info;
extern Logger debug;
extern Logger radio;
extern Logger trace;

#endif /* INCLUDE_CORE_LOGGING_HPP_ */

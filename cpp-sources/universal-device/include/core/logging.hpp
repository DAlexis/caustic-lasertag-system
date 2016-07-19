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

#include "os-wrappers.hpp"
#include "hal/uart.hpp"
#include <string.h>
#include <list>

class Loggers
{
	friend class Logger;
	friend class ScopedTag;
public:
	static void initLoggers(uint8_t portNumber);
	static bool isInitialized();
	static IUARTManager* __attribute__((always_inline)) getUsart() { return uart; }
	static Mutex loggersMutex;
	static uint32_t write(const char* buf, size_t nbyte);
private:
	static void infoOnOff(const char* arg);
	static void debugOnOff(const char* arg);
	static void radioOnOff(const char* arg);
	static void traceOnOff(const char* arg);

	static std::list<const char*> tags;
	static IUARTManager *uart;
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
			ScopedLock<Mutex> lock(Loggers::loggersMutex);
			Loggers::uart->transmitSync((uint8_t*)"\n[", 2);
			Loggers::uart->transmitSync((uint8_t*)m_loggerName, strlen(m_loggerName));
			Loggers::uart->transmitSync((uint8_t*)"] ", 2);
			for (auto it = Loggers::tags.begin(); it != Loggers::tags.end(); it++)
			{
				Loggers::uart->transmitSync((uint8_t*)*it, strlen(*it));
				Loggers::uart->transmitSync((uint8_t*)"|", 1);
			}
			Loggers::uart->transmitSync((uint8_t*)" ", 1);
		}

		return m_unnamedLogger << data;
	}

private:
	struct LoggerUnnamed
	{
		constexpr static unsigned int numbersBufferSize = 20;
		LoggerUnnamed& operator<<(const char* str);
		LoggerUnnamed& operator<<(const std::string& str);
		LoggerUnnamed& operator<<(int d);
		LoggerUnnamed& operator<<(unsigned int d);
		LoggerUnnamed& operator<<(uint32_t d);
		LoggerUnnamed& operator<<(float f);
		LoggerUnnamed& operator<<(double f);
		LoggerUnnamed& operator<<(bool b);
		bool enabled = false;
	};

	LoggerUnnamed m_unnamedLogger;
	const char* m_loggerName;
	bool m_enabled = false;
};

class ScopedTag
{
public:
	ScopedTag(const char* newTag);
	~ScopedTag();

private:
	const char* m_oldTag = nullptr;
};

extern Logger error;
extern Logger warning;
extern Logger info;
extern Logger debug;
extern Logger radio;
extern Logger trace;




#endif /* INCLUDE_CORE_LOGGING_HPP_ */

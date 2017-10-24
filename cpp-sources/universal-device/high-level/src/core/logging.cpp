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


#include "core/logging.hpp"
#include "core/string-utils.hpp"
#include "core/diagnostic.hpp"
#include "core/os-wrappers.hpp"
#include "newlib-driver.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>

std::list<const char*> Loggers::tags;

bool Loggers::m_isInitialized = false;
char* Loggers::m_tmpBuffer = nullptr;
uint16_t Loggers::m_tmpBufferCurrent = 0;
static Mutex loggersMutex;

Logger error  ("E  ERROR");
Logger warning(" W warn ");
Logger info   ("   info ");
Logger debug  ("   debug");
Logger radio  ("   radio");
Logger trace  ("   trace");

void Loggers::initLoggers()
{
	error.enable(true);
	warning.enable(true);
	info.enable(true);
	m_isInitialized = true;
	if (m_tmpBuffer != nullptr)
	{
	    _write(STDOUT_FILENO, m_tmpBuffer, m_tmpBufferCurrent);
		delete[] m_tmpBuffer;
		m_tmpBuffer = nullptr;
	}
}

bool Loggers::isInitialized()
{
	return m_isInitialized;
}

//////////////////////
// LoggerUnnamed
Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(const char* str)
{
	if (enabled)
	{
		_write(STDOUT_FILENO, (char*)str, strlen(str));
	}
	return *this;
}

Logger::LoggerUnnamed&  Logger::LoggerUnnamed::operator<<(const std::string& str)
{
	*this << str.c_str();
	return *this;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(int d)
{
	char buffer[numbersBufferSize];
	sprintf(buffer, "%d", d);
	return *this << buffer;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(unsigned int d)
{
	char buffer[numbersBufferSize];
	sprintf(buffer, "%u", d);
	return *this << buffer;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(uint32_t d)
{
	char buffer[numbersBufferSize];
	int i=numbersBufferSize-1;
	buffer[i--] = '\0';
	for (; d != 0; i--, d /= 10)
		buffer[i] = d%10 + '0';

	return *this << &(buffer[i+1]);
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(uint64_t d)
{
	char buffer[numbersBufferSize];
	int i=numbersBufferSize-1;
	buffer[i--] = '\0';
	for (; d != 0; i--, d /= 10)
		buffer[i] = d%10 + '0';

	return *this << &(buffer[i+1]);
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(float f)
{
	char buffer[numbersBufferSize];
	int size = std::min(5, (int)numbersBufferSize-1);
	floatToString(buffer, f, size, 2);
	buffer[size] = '\0';
	return *this << buffer;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(double f)
{
	char buffer[numbersBufferSize];
	int size = std::min(5, (int)numbersBufferSize-1);
	doubleToString(buffer, f, size, 2);
	buffer[size] = '\0';
	return *this << buffer;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(bool b)
{
	if (b)
		return *this << "true";
	else
		return *this << "false";
}

//////////////////////
// Logger
void Logger::enable(bool enabled)
{
	m_enabled = enabled;
	m_unnamedLogger.enabled = enabled;
}

bool Logger::isEnabled()
{
	return m_enabled;
}

void Logger::writePrefix()
{
	char buffer[50];
	sprintf(buffer,
			"\n[%s heap=%d task=%x]",
			m_loggerName,
			(int)SystemMonitor::instance().getFreeheap(),
			(unsigned int) (Kernel::instance().getCurrentTaskId() & 0xFFFFF)
	);
	_write(STDOUT_FILENO, buffer, strlen(buffer));
	for (auto it = Loggers::tags.begin(); it != Loggers::tags.end(); it++)
	{
		_write(STDOUT_FILENO, (char*)*it, strlen(*it));
		_write(STDOUT_FILENO, (char*)"|", 1);
	}
	_write(STDOUT_FILENO, (char*)" ", 1);
}

extern "C" {
	void lockOutputPort()
	{
		if (Loggers::isInitialized())
			loggersMutex.lock();
	}

	void unlockOutputPort()
	{
		if (Loggers::isInitialized())
			loggersMutex.unlock();
	}
}

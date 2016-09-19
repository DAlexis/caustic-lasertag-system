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
#include <stdio.h>
#include <string.h>
#include <algorithm>


IUARTManager *Loggers::uart = nullptr;
std::list<const char*> Loggers::tags;

bool Loggers::m_isInitialized = false;
char* Loggers::m_tmpBuffer = nullptr;
uint16_t Loggers::m_tmpBufferCurrent = 0;

Mutex Loggers::loggersMutex;

Logger error  ("ERROR");
Logger warning("warn ");
Logger info   ("info ");
Logger debug  ("debug");
Logger radio  ("radio");
Logger trace  ("trace");

void Loggers::initLoggers(uint8_t portNumber)
{
	uart = UARTSFactory->create();
	uart->init(portNumber, 921600);
	error.enable(true);
	warning.enable(true);
	info.enable(true);
	m_isInitialized = true;
	if (m_tmpBuffer != nullptr)
	{
		getUsart()->transmitSync((uint8_t*)m_tmpBuffer, m_tmpBufferCurrent);
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
		ScopedLock<Mutex> lock(Loggers::loggersMutex);
		Loggers::uart->transmitSync((uint8_t*)str, strlen(str));
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

uint32_t Loggers::write(const char* buf, size_t nbyte)
{

	//ScopedLock lock(Loggers::loggersMutex);
	if (isInitialized())
	{
		getUsart()->transmitSync((uint8_t*)buf, nbyte);
	}
	else
	{
		if (m_tmpBuffer == nullptr)
			m_tmpBuffer = new char[tmpBufferSize];

		if (tmpBufferSize-m_tmpBufferCurrent > nbyte)
		{
			memcpy(&m_tmpBuffer[m_tmpBufferCurrent], buf, nbyte);
			m_tmpBufferCurrent += nbyte;
		}
	}
	return nbyte;
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

extern "C" ssize_t
_write (int fd __attribute__((unused)), const char* buf __attribute__((unused)),
	size_t nbyte __attribute__((unused)))
{
	return Loggers::write(buf, nbyte);
}

extern "C" int __attribute__((weak))
_fstat(int fildes __attribute__((unused)),
    struct stat* st __attribute__((unused)))
{
  //errno = ENOSYS;
  return -1;
}

extern "C" int __attribute__((weak))
_lseek(int file __attribute__((unused)), int ptr __attribute__((unused)),
    int dir __attribute__((unused)))
{
  //errno = ENOSYS;
  return -1;
}

extern "C" int __attribute__((weak))
_close(int fildes __attribute__((unused)))
{
  //errno = ENOSYS;
  return -1;
}

extern "C" int __attribute__((weak))
_read(int file __attribute__((unused)), char* ptr __attribute__((unused)),
    int len __attribute__((unused)))
{
  //errno = ENOSYS;
  return -1;
}

extern "C" int __attribute__((weak))
_isatty(int file __attribute__((unused)))
{
  //errno = ENOSYS;
  return 0;
}

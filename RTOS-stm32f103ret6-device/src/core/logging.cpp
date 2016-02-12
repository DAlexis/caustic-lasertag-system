/*
 * logging.cpp
 *
 *  Created on: 21 апр. 2015 г.
 *      Author: alexey
 */

#include "core/logging.hpp"
#include <stdio.h>
#include <string.h>


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
	uart->init(1, 921600);
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
	// I'm not sure that %u is the best solution
	sprintf(buffer, "%u", d);
	return *this << buffer;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(float f)
{
	char buffer[numbersBufferSize];
	sprintf(buffer, "%f", f);
	return *this << buffer;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(double f)
{
	char buffer[numbersBufferSize];
	sprintf(buffer, "%lf", f);
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

//////////////////////
// ScoppedTag
ScopedTag::ScopedTag(const char* newTag)
{
	/*
	m_oldTag = Loggers::tag;
	Loggers::tag = newTag;*/
	//Loggers::tags.push_back(newTag);
}

ScopedTag::~ScopedTag()
{
	//Loggers::tag = m_oldTag;
	//Loggers::tags.pop_back();
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

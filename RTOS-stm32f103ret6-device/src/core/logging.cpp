/*
 * logging.cpp
 *
 *  Created on: 21 апр. 2015 г.
 *      Author: alexey
 */

#include "core/logging.hpp"
#include <stdio.h>
#include <string.h>

UART_HandleTypeDef Loggers::huart;
//const char* Loggers::tag = nullptr;
std::list<const char*> Loggers::tags;

Mutex Loggers::loggersMutex;

Logger error  ("ERROR");
Logger warning("warn ");
Logger info   ("info ");
Logger debug  ("debug");
Logger radio  ("radio");
Logger trace  ("trace");

void Loggers::initLoggers(uint8_t portNumber)
{
	huart.Instance = USART1;
	huart.Init.BaudRate = 921600;
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart);
	error.enable(true);
	warning.enable(true);
	info.enable(true);
}

//////////////////////
// LoggerUnnamed
Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(const char* str)
{
	if (enabled)
	{
		ScopedLock lock(Loggers::loggersMutex);
		HAL_UART_Transmit(Loggers::getUsart(), (uint8_t*)str, strlen(str), 100000);
	}
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

//////////////////////
// Logger
void Logger::enable(bool enabled)
{
	m_enabled = enabled;
	m_unnamedLogger.enabled = enabled;
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
	//ScopedLock lock(Loggers::loggersMutex);
	HAL_UART_Transmit(Loggers::getUsart(), (uint8_t*)buf, nbyte, 100000);
	return nbyte;
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

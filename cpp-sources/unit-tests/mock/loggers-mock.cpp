/*
 * loggers-mock.cpp
 *
 *  Created on: 17 окт. 2017 г.
 *      Author: dalexies
 */

#include "core/logging.hpp"

bool Loggers::m_isInitialized = false;

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
}

bool Loggers::isInitialized()
{
	return m_isInitialized;
}

//////////////////////
// LoggerUnnamed
Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(const char* str)
{
	return *this;
}

Logger::LoggerUnnamed&  Logger::LoggerUnnamed::operator<<(const std::string& str)
{
	return *this;
}


Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(void* p)
{
	return *this;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(int d)
{
	return *this;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(unsigned int d)
{
	return *this;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(uint64_t d)
{
	return *this;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(float f)
{
	return *this;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(double f)
{
	return *this;
}

Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(bool b)
{
	return *this;
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

}


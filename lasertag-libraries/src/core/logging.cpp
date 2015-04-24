/*
 * logging.cpp
 *
 *  Created on: 21 апр. 2015 г.
 *      Author: alexey
 */

#include "core/logging.hpp"
#include <stdio.h>

IUSARTManager* Loggers::m_usart = nullptr;
//const char* Loggers::tag = nullptr;
std::list<const char*> Loggers::tags;

Logger error  ("ERROR");
Logger warning("warn ");
Logger info   ("info ");
Logger debug  ("debug");
Logger radio  ("radio");
Logger trace  ("trace");

void Loggers::initLoggers(uint8_t portNumber)
{
	m_usart = USARTS->getUSART(portNumber);
	m_usart->init(921600);
	error.enable(true);
	warning.enable(true);
	info.enable(true);
}

//////////////////////
// LoggerUnnamed
Logger::LoggerUnnamed& Logger::LoggerUnnamed::operator<<(const char* str)
{
	if (enabled)
		Loggers::getUsart()->syncWrite(str);
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
	Loggers::tags.push_back(newTag);
}

ScopedTag::~ScopedTag()
{
	//Loggers::tag = m_oldTag;
	Loggers::tags.pop_back();
}

/*
 * logging.hpp
 *
 *  Created on: 21 апр. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_CORE_LOGGING_HPP_
#define INCLUDE_CORE_LOGGING_HPP_

#include "os-wrappers.hpp"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <list>

class Loggers
{
	friend class Logger;
	friend class ScopedTag;
public:
	static void initLoggers(uint8_t portNumber);
	static UART_HandleTypeDef* __attribute__((always_inline)) getUsart() { return &huart; }

	static Mutex loggersMutex;
private:
	static void infoOnOff(const char* arg);
	static void debugOnOff(const char* arg);
	static void radioOnOff(const char* arg);
	static void traceOnOff(const char* arg);

	static std::list<const char*> tags;
	//static const char* tag;
	static UART_HandleTypeDef huart;
};

class Logger
{
private:
	class LoggerUnnamed;

public:
	Logger(const char* loggerName) : m_loggerName(loggerName) {}

	void enable(bool enabled = true);

	template<class T>
	LoggerUnnamed& operator<<(T&& data)
	{
		if (m_enabled)
		{
			ScopedLock lock(Loggers::loggersMutex);
			HAL_UART_Transmit(Loggers::getUsart(), (uint8_t*)"\n[", 2, 100000);
			HAL_UART_Transmit(Loggers::getUsart(), (uint8_t*)m_loggerName, strlen(m_loggerName), 100000);
			HAL_UART_Transmit(Loggers::getUsart(), (uint8_t*)"] ", 2, 100000);
			for (auto it = Loggers::tags.begin(); it != Loggers::tags.end(); it++)
			{
				HAL_UART_Transmit(Loggers::getUsart(), (uint8_t*)*it, strlen(*it), 100000);
				HAL_UART_Transmit(Loggers::getUsart(), (uint8_t*)"|", 1, 100000);
			}
			HAL_UART_Transmit(Loggers::getUsart(), (uint8_t*)" ", 1, 100000);
		}

		return m_unnamedLogger << data;
	}

private:
	struct LoggerUnnamed
	{
		constexpr static unsigned int numbersBufferSize = 20;
		LoggerUnnamed& operator<<(const char* str);
		LoggerUnnamed& operator<<(int d);
		LoggerUnnamed& operator<<(unsigned int d);
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

/*
 * diagnostic.hpp
 *
 *  Created on: 27 марта 2016 г.
 *      Author: alexey
 */

#ifndef INCLUDE_CORE_DIAGNOSTIC_HPP_
#define INCLUDE_CORE_DIAGNOSTIC_HPP_

#include "utils/macro.hpp"
#include "hal/system-clock.hpp"
#include "core/os-wrappers.hpp"

#include <vector>
#include <list>
#include <stdint.h>

class Stager
{
public:
	Stager(const char* name = "");
	void stage(const char* description);
	void setName(const char* name);
	void printStages() const;

private:
	struct StagerRec
	{
		Time t =0;
		const char* description = "";
	};
	constexpr static uint8_t ringBufferSize = 10;

	StagerRec m_stagesRingBuffer[ringBufferSize];
	uint8_t m_pointer = 0;

	const char* m_name;
};

class SystemMonitor
{
public:
	void registerStager(const Stager* stager);
	void printSummary();
	void run();

	SIGLETON_IN_CLASS(SystemMonitor)
private:
	SystemMonitor();
	void printTime() const;
	void printVoltage() const;
	void printStagers() const;
	void printOSDetails();

	std::list<const Stager*> m_stagers;
	TaskCycled m_task;
	char m_strBuffer[400];
};




#endif /* INCLUDE_CORE_DIAGNOSTIC_HPP_ */

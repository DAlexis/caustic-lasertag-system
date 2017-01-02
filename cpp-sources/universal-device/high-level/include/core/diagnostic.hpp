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

#ifndef INCLUDE_CORE_DIAGNOSTIC_HPP_
#define INCLUDE_CORE_DIAGNOSTIC_HPP_

#include "utils/macro.hpp"
#include "hal/system-clock.hpp"
#include "core/os-wrappers.hpp"

#include <vector>
#include <list>
#include <stdint.h>

#ifdef DEBUG
	#define STAGER Stager
#else
	#define STAGER StagerStub
#endif

class StagerStub
{
public:
	StagerStub(const char* name = "", Time aliveCheckPeriod = 0) { UNUSED_ARG(name); UNUSED_ARG(aliveCheckPeriod); }
	void stage(const char* description) { UNUSED_ARG(description); }
	void setName(const char* name) { UNUSED_ARG(name); }
	void printStages() const {}
};

class Stager
{
public:
	Stager(const char* name = "", Time aliveCheckPeriod = 0);
	void stage(const char* description);
	void setName(const char* name);
	void printStages() const;

private:
	struct StagerRec
	{
		Time t = 0;
		const char* description = "";
	};
	constexpr static uint8_t ringBufferSize = 10;

	StagerRec m_stagesRingBuffer[ringBufferSize];
	uint8_t m_pointer = 0;

	const char* m_name;
	Time m_aliveCheckPeriod = 0;
};

class SystemMonitor
{
public:
	void registerStager(const Stager* stager);
	void printSummary();
	void run();

	SINGLETON_IN_CLASS(SystemMonitor)
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

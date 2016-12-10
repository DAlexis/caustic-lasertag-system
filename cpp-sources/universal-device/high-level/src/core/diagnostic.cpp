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


#include "core/diagnostic.hpp"
#include "core/logging.hpp"
#include "core/power-monitor.hpp"
#include "core/debug-settings.hpp"
#include "hal/rtc.hpp"
#include "string.h"

SINGLETON_IN_CPP(SystemMonitor)

Stager::Stager(const char* name, Time aliveCheckPeriod) :
	m_name(name),
	m_aliveCheckPeriod(aliveCheckPeriod)
{
	for (int i=0; i<ringBufferSize; i++)
		stage("Nothing");

	SystemMonitor::instance().registerStager(this);
}

void Stager::stage(const char* description)
{
	m_stagesRingBuffer[m_pointer].description = description;
	m_stagesRingBuffer[m_pointer].t = systemClock->getTime();

	if (m_pointer < ringBufferSize-1)
		m_pointer++;
	else
		m_pointer = 0;
}

void Stager::setName(const char* name)
{
	m_name = name;
}

void Stager::printStages() const
{
	uint8_t current = m_pointer != 0 ? m_pointer-1 : ringBufferSize-1;

	debug << "Stages history: " << m_name;

	if (m_aliveCheckPeriod != 0)
	{
		if (m_stagesRingBuffer[current].t - systemClock->getTime() > m_aliveCheckPeriod)
			debug << "Seems alive";
		else
			debug << "Seems dead!";
	}

	for (uint8_t i=0; i<ringBufferSize; i++)
	{
		debug << i << ": [" << m_stagesRingBuffer[current].t << "] " << m_stagesRingBuffer[current].description;
		if (current != 0)
			current--;
		else
			current = ringBufferSize-1;
	}
}

SystemMonitor::SystemMonitor()
{
	m_task.setName("SysMon");
	m_task.setStackSize(256);
	m_task.setTask([this]{ printSummary(); });
}

void SystemMonitor::registerStager(const Stager* stager)
{
	m_stagers.push_back(stager);
}

void SystemMonitor::run()
{
#ifndef DBG_SYSTEM_MONITOR_DISABLE
	m_task.run(0, 5000, 5000, 0);
#endif
}

void SystemMonitor::printSummary()
{
	printTime();
	printVoltage();
	printOSDetails();
	printStagers();
}

void SystemMonitor::printTime() const
{
    debug << "RTC disabled";
    /*
	RTCTime t;
	RTCManager->getTime(t);
	RTCDate d;
	RTCManager->getDate(d);
	debug   << "I'm alive, " << t.hours << ":" << t.mins << ":" << t.secs << "; "
			<< d.day << "." << d.month << "." << d.year;*/
}

void SystemMonitor::printVoltage() const
{
	debug << "VCC = " << PowerMonitor::instance().supplyVoltage
			<< ", " << PowerMonitor::instance().chargePercent << "%";
}

void SystemMonitor::printStagers() const
{
	debug << "Stages of components:";
	for (auto it = m_stagers.begin(); it != m_stagers.end(); it++)
	{
		(*it)->printStages();
	}
}

void SystemMonitor::printOSDetails()
{
	debug << "OS info:";/*
	int count = Kernel::instance().getTasksCount();
	debug << "Running tasks: " << count;
	vTaskList(m_strBuffer);
	debug << "Task stats:";
	debug << "   Name\t\tstate\tprior\tstack\tnum";
	int i=0;
	int lineBegin = 0;
	for (; m_strBuffer[i] != '\0'; i++)
	{
		if (m_strBuffer[i] == '\n')
		{
			m_strBuffer[i] = '\0';
			debug << "   " << &m_strBuffer[lineBegin];
			lineBegin = i+1;
		}
	}

	debug << "FreeRTOS free heap: " << xPortGetFreeHeapSize() << ", min: " << xPortGetMinimumEverFreeHeapSize();*/
}


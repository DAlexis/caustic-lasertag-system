/*
 * diagnostic.cpp
 *
 *  Created on: 27 марта 2016 г.
 *      Author: alexey
 */

#include "core/diagnostic.hpp"
#include "core/logging.hpp"
#include "core/power-monitor.hpp"
#include "hal/rtc.hpp"
#include "string.h"

SINGLETON_IN_CPP(SystemMonitor)

Stager::Stager(const char* name) :
	m_name(name)
{
	for (int i=0; i<ringBufferSize; i++)
		stage("Nothing");

	memset(m_stagesRingBuffer, 0, sizeof(m_stagesRingBuffer[0]) * ringBufferSize);
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
	uint8_t current = m_pointer;
	debug << "Stages history: " << m_name;
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
	m_task.run(0, 5000, 5000, 0);
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
	RTCTime t;
	RTCManager->getTime(t);
	RTCDate d;
	RTCManager->getDate(d);
	debug   << "I'm alive, " << t.hours << ":" << t.mins << ":" << t.secs << "; "
			<< d.day << "." << d.month << "." << d.year;
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
	debug << "OS info:";
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

	debug << "FreeRTOS free heap: " << xPortGetFreeHeapSize() << ", min: " << xPortGetMinimumEverFreeHeapSize();
}


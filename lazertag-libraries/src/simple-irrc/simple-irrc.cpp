/*
 * simple-irrc.cpp
 *
 *  Created on: 23 февр. 2015 г.
 *      Author: alexey
 */

#include "simple-irrc/simple-irrc.hpp"
#include "core/scheduler.hpp"

#include <stdio.h>

void SimpleIRRC::init()
{
	printf("Simple IR remote control initialization...\n");
	m_respawnButton = nullptr;
	m_killButton = nullptr;
	m_teamRedButton = nullptr;
	m_teamBlueButton = nullptr;
	m_decreaseHPButton = nullptr;
	m_increaseHPButton = nullptr;


	printf("Simple IR remote control ready to use\n");
}

void SimpleIRRC::interrogate()
{
	m_respawnButton = ButtonsPool::instance().getButtonManager(respawnButtonPort, respawnButtonPin);
	m_respawnButton->setAutoRepeat(false);
	m_respawnButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_respawnButton->setCallback(std::bind(&SimpleIRRC::respawnButonCb, this, std::placeholders::_1));
	m_respawnButton->turnOn();
	Scheduler::instance().addTask(std::bind(&ButtonManager::interrogate, m_respawnButton), false, buttonsInterrogationPeriod);

	m_killButton = ButtonsPool::instance().getButtonManager(killButtonPort, killButtonPin);
	m_killButton->setAutoRepeat(false);
	m_killButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_killButton->setCallback(std::bind(&SimpleIRRC::killButtonCb, this, std::placeholders::_1));
	m_killButton->turnOn();
	Scheduler::instance().addTask(std::bind(&ButtonManager::interrogate, m_killButton), false, buttonsInterrogationPeriod);

	m_teamRedButton = ButtonsPool::instance().getButtonManager(teamRedButtonPort, teamRedButtonPin);
	m_teamRedButton->setAutoRepeat(false);
	m_teamRedButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_teamRedButton->setCallback(std::bind(&SimpleIRRC::teamRedButtonCb, this, std::placeholders::_1));
	m_teamRedButton->turnOn();
	Scheduler::instance().addTask(std::bind(&ButtonManager::interrogate, m_teamRedButton), false, buttonsInterrogationPeriod);

	m_teamBlueButton = ButtonsPool::instance().getButtonManager(teamBlueButtonPort, teamBlueButtonPin);
	m_teamBlueButton->setAutoRepeat(false);
	m_teamBlueButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_teamBlueButton->setCallback(std::bind(&SimpleIRRC::teamBlueButtonCb, this, std::placeholders::_1));
	m_teamBlueButton->turnOn();
	Scheduler::instance().addTask(std::bind(&ButtonManager::interrogate, m_teamBlueButton), false, buttonsInterrogationPeriod);

	m_decreaseHPButton = ButtonsPool::instance().getButtonManager(decreaseHPButtonPort, decreaseHPButtonPin);
	m_decreaseHPButton->setAutoRepeat(false);
	m_decreaseHPButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_decreaseHPButton->setCallback(std::bind(&SimpleIRRC::decreaseHPButtonCb, this, std::placeholders::_1));
	m_decreaseHPButton->turnOn();
	Scheduler::instance().addTask(std::bind(&ButtonManager::interrogate, m_decreaseHPButton), false, buttonsInterrogationPeriod);

	m_increaseHPButton = ButtonsPool::instance().getButtonManager(increaseHPButtonPort, increaseHPButtonPin);
	m_increaseHPButton->setAutoRepeat(false);
	m_increaseHPButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_increaseHPButton->setCallback(std::bind(&SimpleIRRC::increaseHPButtonCb, this, std::placeholders::_1));
	m_increaseHPButton->turnOn();
	Scheduler::instance().addTask(std::bind(&ButtonManager::interrogate, m_increaseHPButton), false, buttonsInterrogationPeriod);
}

void SimpleIRRC::respawnButonCb(bool)
{
}

void SimpleIRRC::killButtonCb(bool)
{
}

void SimpleIRRC::teamRedButtonCb(bool)
{
}

void SimpleIRRC::teamBlueButtonCb(bool)
{
}

void SimpleIRRC::decreaseHPButtonCb(bool)
{
}

void SimpleIRRC::increaseHPButtonCb(bool)
{
}
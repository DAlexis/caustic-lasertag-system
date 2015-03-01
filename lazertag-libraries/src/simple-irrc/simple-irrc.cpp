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


	m_respawnButton = ButtonsPool::instance().getButtonManager(respawnButtonPort, respawnButtonPin);
	m_respawnButton->setAutoRepeat(false);
	m_respawnButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_respawnButton->setCallback(std::bind(&SimpleIRRC::respawnButonCb, this, std::placeholders::_1));
	m_respawnButton->turnOn();

	m_killButton = ButtonsPool::instance().getButtonManager(killButtonPort, killButtonPin);
	m_killButton->setAutoRepeat(false);
	m_killButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_killButton->setCallback(std::bind(&SimpleIRRC::killButtonCb, this, std::placeholders::_1));
	m_killButton->turnOn();

	m_teamRedButton = ButtonsPool::instance().getButtonManager(teamRedButtonPort, teamRedButtonPin);
	m_teamRedButton->setAutoRepeat(false);
	m_teamRedButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_teamRedButton->setCallback(std::bind(&SimpleIRRC::teamRedButtonCb, this, std::placeholders::_1));
	m_teamRedButton->turnOn();

	m_teamBlueButton = ButtonsPool::instance().getButtonManager(teamBlueButtonPort, teamBlueButtonPin);
	m_teamBlueButton->setAutoRepeat(false);
	m_teamBlueButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_teamBlueButton->setCallback(std::bind(&SimpleIRRC::teamBlueButtonCb, this, std::placeholders::_1));
	m_teamBlueButton->turnOn();

	m_decreaseHPButton = ButtonsPool::instance().getButtonManager(decreaseHPButtonPort, decreaseHPButtonPin);
	m_decreaseHPButton->setAutoRepeat(false);
	m_decreaseHPButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_decreaseHPButton->setCallback(std::bind(&SimpleIRRC::decreaseHPButtonCb, this, std::placeholders::_1));
	m_decreaseHPButton->turnOn();

	m_increaseHPButton = ButtonsPool::instance().getButtonManager(increaseHPButtonPort, increaseHPButtonPin);
	m_increaseHPButton->setAutoRepeat(false);
	m_increaseHPButton->setRepeatPeriod(buttonsRepeatPeriod);
	m_increaseHPButton->setCallback(std::bind(&SimpleIRRC::increaseHPButtonCb, this, std::placeholders::_1));
	m_increaseHPButton->turnOn();

	m_mt2.init();
	m_mt2.setChannel(2);
	Scheduler::instance().addTask(std::bind(&SimpleIRRC::interrogate, this), false, buttonsInterrogationPeriod);
	printf("Simple IR remote control ready to use\n");
}

void SimpleIRRC::interrogate()
{
	m_respawnButton->interrogate();
	m_killButton->interrogate();
	m_teamRedButton->interrogate();
	m_teamBlueButton->interrogate();
	m_decreaseHPButton->interrogate();
	m_increaseHPButton->interrogate();
}

void SimpleIRRC::respawnButonCb(bool)
{
	printf("Respawn\n");
	m_mt2.respawn();
}

void SimpleIRRC::killButtonCb(bool)
{
	printf("Kill\n");
	m_mt2.adminKill();
}

void SimpleIRRC::teamRedButtonCb(bool)
{
	printf("Team id to 0\n");
	printf("TODO\n");
}

void SimpleIRRC::teamBlueButtonCb(bool)
{
	printf("Team id to 1\n");
	printf("TODO\n");
}

void SimpleIRRC::decreaseHPButtonCb(bool)
{
	printf("Decrease hp\n");
	printf("TODO\n");
}

void SimpleIRRC::increaseHPButtonCb(bool)
{
	printf("Increase hp\n");
	printf("TODO\n");
}

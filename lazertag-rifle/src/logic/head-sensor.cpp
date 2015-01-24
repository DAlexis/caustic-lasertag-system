/*
 * head-sensor.cpp
 *
 *  Created on: 24 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/head-sensor.hpp"
#include "core/scheduler.hpp"
#include "hal/ext-interrupts.hpp"

#include <stdio.h>

HeadSensor::HeadSensor()
{
}

void HeadSensor::configure()
{
	IExternalInterruptManager *exti = EXTIS->getEXTI(0);
	exti->init(0);
	exti->turnOn();

	m_mainSensor.setShortMessageCallback(std::bind(&HeadSensor::shotCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	m_mainSensor.init(exti);
	m_mainSensor.enableDebug(true);

	Scheduler::instance().addTask(std::bind(&MilesTag2Receiver::interrogate, &m_mainSensor), false, 50000, 30000);
	printf("Head sensor ready to use\n");
}

void HeadSensor::shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage)
{
	printf("Cought a shot: team %d, player %d, damage: %d\n", teamId, playerId, damage);
}

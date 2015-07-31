/*
 * kill-zones.cpp
 *
 *  Created on: 07 мая 2015 г.
 *      Author: alexey
 */

#include "head-sensor/kill-zones.hpp"
#include "core/logging.hpp"

KillZonesManager::KillZonesManager(
		const FloatParameter& zone1DamageCoeff,
		const FloatParameter& zone2DamageCoeff,
		const FloatParameter& zone3DamageCoeff,
		const FloatParameter& zone4DamageCoeff,
		const FloatParameter& zone5DamageCoeff,
		const FloatParameter& zone6DamageCoeff
	)
{
	m_killZones[0].zoneDamageCoeff = &zone1DamageCoeff;
	m_killZones[1].zoneDamageCoeff = &zone2DamageCoeff;
	m_killZones[2].zoneDamageCoeff = &zone3DamageCoeff;
	m_killZones[3].zoneDamageCoeff = &zone4DamageCoeff;
	m_killZones[4].zoneDamageCoeff = &zone5DamageCoeff;
	m_killZones[5].zoneDamageCoeff = &zone6DamageCoeff;

	m_interrogateTask.setStackSize(1024);
	m_interrogateTask.setTask(std::bind(&KillZonesManager::interrogate, this));
	m_interrogateTask.run(50, 2, 2);
}

void KillZonesManager::setCallback(DamageCallback callback)
{
	m_callback = callback;
}

void KillZonesManager::enableKillZone(uint8_t zone, IIOPin *input, IIOPin* vibro)
{
	if (vibro)
	{
		vibro->switchToOutput();
		vibro->reset();
	}
	m_killZones[zone].vibro = vibro;
	m_killZones[zone].killZone = new MilesTag2Receiver;

	m_killZones[zone].killZone->setShortMessageCallback(std::bind(
			&KillZonesManager::IRReceiverShotCallback_ISR,
			this,
			zone,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
			));
	m_killZones[zone].killZone->init(input);
	m_killZones[zone].killZone->turnOn();
}

void KillZonesManager::IRReceiverShotCallback_ISR(uint8_t zoneId, unsigned int teamId, unsigned int playerId, unsigned int damage)
{
	damage *= *(m_killZones[zoneId].zoneDamageCoeff);
	if (m_lastDamageMoment == 0 || m_maxDamage < damage)
	{
		m_lastDamageMoment = systemClock->getTime();
		m_maxDamage = damage;
		m_teamId = teamId;
		m_playerId = playerId;
	}

	// Enabling vibroengine if exists
	if (m_killZones[zoneId].vibro)
	{
		m_killZones[zoneId].vibro->set();
		m_killZones[zoneId].vibrationStopTime = systemClock->getTime() + vibroPeriod;
	}

	//info << "Kill zone activated";
}

void KillZonesManager::interrogate()
{
	for (int i=0; i<killZonesMaxCount; i++)
	{
		if (m_killZones[i].killZone)
		{
			m_killZones[i].killZone->interrogate();
		}
		// Stopping vibration
		Time currentTime = systemClock->getTime();
		if (m_killZones[i].vibrationStopTime <= currentTime)
		{
			m_killZones[i].vibro->reset();
		}
	}

	// If it is time to call shot callback
	if (
			m_lastDamageMoment != 0 // We have at least one shot
			&& systemClock->getTime() - m_lastDamageMoment > callbackDelay // We have waited for enough time
		)
	{
		if (!m_callback)
		{
			error << "Zones manager callback not set!\n";
		} else {
			m_callback(m_teamId, m_playerId, m_maxDamage);
		}
		m_lastDamageMoment = 0;
	}
}

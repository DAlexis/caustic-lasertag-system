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
	m_zoneDamageCoeff[0] = &zone1DamageCoeff;
	m_zoneDamageCoeff[1] = &zone2DamageCoeff;
	m_zoneDamageCoeff[2] = &zone3DamageCoeff;
	m_zoneDamageCoeff[3] = &zone4DamageCoeff;
	m_zoneDamageCoeff[4] = &zone5DamageCoeff;
	m_zoneDamageCoeff[5] = &zone6DamageCoeff;
	for (int i=0; i<killZonesMaxCount; i++)
	{
		m_vibro[i] = nullptr;
		m_killZone[i] = nullptr;
	}

	m_vibrationStopTask.setStackSize(128);
	m_vibrationStopTask.setTask(std::bind(&KillZonesManager::stopVibrate, this));

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
	m_vibro[zone] = vibro;
	m_killZone[zone] = new MilesTag2Receiver;

	m_killZone[zone]->setShortMessageCallback(std::bind(
			&KillZonesManager::IRReceiverShotCallback_ISR,
			this,
			zone,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
			));
	m_killZone[zone]->init(input);
	m_killZone[zone]->turnOn();
}

void KillZonesManager::IRReceiverShotCallback_ISR(uint8_t zoneId, unsigned int teamId, unsigned int playerId, unsigned int damage)
{
	damage *= *(m_zoneDamageCoeff[zoneId]);
	if (m_lastDamageMoment == 0 || m_maxDamage < damage)
	{
		m_lastDamageMoment = systemClock->getTime();
		m_maxDamage = damage;
		m_teamId = teamId;
		m_playerId = playerId;
	}

	//vibrate(zoneId);
	//info << "Kill zone activated";
}

void KillZonesManager::interrogate()
{
	for (int i=0; i<killZonesMaxCount; i++)
	{
		if (m_killZone[i])
		{
			m_killZone[i]->interrogate();
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

void KillZonesManager::vibrate(uint8_t zone)
{
	if (!m_vibro[zone])
		return;

	m_vibro[zone]->set();
	m_vibrationStopTask.stopUnsafe();
	m_vibrationStopTask.run(vibroPeriod);
}

void KillZonesManager::stopVibrate()
{
	for (int zone=0; zone<killZonesMaxCount; zone++)
		m_vibro[zone]->reset();
}

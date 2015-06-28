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
		m_vibro[i] = nullptr;
	m_callDamageCallbackTask.setStackSize(256);
	m_callDamageCallbackTask.setTask(std::bind(&KillZonesManager::callDamageCallback, this));

	m_vibrationStopTask.setStackSize(128);
	m_vibrationStopTask.setTask(std::bind(&KillZonesManager::stopVibrate, this));
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

	m_killZone[zone].setShortMessageCallback(std::bind(
			&KillZonesManager::IRReceiverShotCallback,
			this,
			zone,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
			));
	m_killZone[zone].init(input);
	m_killZone[zone].turnOn();
}

void KillZonesManager::IRReceiverShotCallback(uint8_t zoneId, unsigned int teamId, unsigned int playerId, unsigned int damage)
{
	damage *= *(m_zoneDamageCoeff[zoneId]);
	vibrate(zoneId);
	if (!m_damageCbScheduled || damage > m_maxDamage)
	{
		m_maxDamage = damage;
		m_teamId = teamId;
		m_playerId = playerId;
	}
	if (!m_damageCbScheduled)
	{
		m_damageCbScheduled = true;
		m_callDamageCallbackTask.run(callbackDelay);
	}
}

void KillZonesManager::callDamageCallback()
{
	m_damageCbScheduled = false;
	if (!m_callback)
	{
		ScopedTag tag("kill-zones-mgr");
		error << "Zones manager callback not set!\n";
		return;
	}
	m_callback(m_teamId, m_playerId, m_maxDamage);
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

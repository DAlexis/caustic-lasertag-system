/*
 * kill-zones.cpp
 *
 *  Created on: 07 мая 2015 г.
 *      Author: alexey
 */

#include "head-sensor/kill-zones.hpp"
#include "core/scheduler.hpp"
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
}

void KillZonesManager::setCallback(DamageCallback callback)
{
	m_callback = callback;
}

void KillZonesManager::enableKillZone(uint8_t zone, IExternalInterruptManager *exti)
{
	m_killZone[zone].setShortMessageCallback(std::bind(
			&KillZonesManager::IRReceiverShotCallback,
			this,
			zone,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
			));
	m_killZone[zone].init(exti);
	m_killZone[zone].turnOn();
}

void KillZonesManager::IRReceiverShotCallback(uint8_t zoneId, unsigned int teamId, unsigned int playerId, unsigned int damage)
{
	damage *= *(m_zoneDamageCoeff[zoneId]);
	if (!m_damageCbScheduled || damage > m_maxDamage)
	{
		m_maxDamage = damage;
		m_teamId = teamId;
		m_playerId = playerId;
	}
	if (!m_damageCbScheduled)
	{
		m_damageCbScheduled = true;
		Scheduler::instance().addTask(std::bind(&KillZonesManager::callDamageCallback, this), true, 0, 0, callbackDelay);
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

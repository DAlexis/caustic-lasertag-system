#include "sensors/kill-zones-manager.hpp"

void KillZonesManager::assignSensorToZone(UintParameter sensorId, UintParameter zoneId)
{
	m_sensorToZone[sensorId] = zoneId;
}

void KillZonesManager::setDamageCoefficient(UintParameter zoneId, FloatParameter& damageCoefficient)
{
	m_damageCoefficient[zoneId] = &damageCoefficient;
}

const std::map<UintParameter, UintParameter>& KillZonesManager::sensorToZone()
{
	return m_sensorToZone;
}

FloatParameter KillZonesManager::getDamageCoefficient(UintParameter sensorId)
{
	auto it = m_sensorToZone.find(sensorId);
	if (it == m_sensorToZone.end())
		return 1.0; // Sensor is not assigned to zone

	auto jt = m_damageCoefficient.find(it->second);
	if (jt == m_damageCoefficient.end())
		return 1.0; // Coefficient is not assigned to zone

	return *jt->second;
}

void KillZonesManager::visitSensorsInZone(UintParameter zoneId, ZoneVisitor visitor)
{
	for (auto it: m_sensorToZone)
		if (it.second == zoneId)
			visitor(it.first);
}

#include "output/illumination.hpp"

LedVibroManager::LedVibroManager(KillZonesManager& mgr) :
	m_killZonesManager(mgr)
{
}

void LedVibroManager::addPoint(IRGBVibroPointPhysical* m_point, UintParameter zoneId, bool zoneWide, bool systemWide)
{
	m_pointsById[m_point->getId()] = m_point;

	auto it = m_zoneWides.find(zoneId);
	if (zoneWide || it == m_zoneWides.end())
	{
		m_zoneWides[zoneId] = m_point;
	}

	if (systemWide || m_systemWide == nullptr)
	{
		m_systemWide = m_point;
	}
}

void LedVibroManager::applyIlluminationSchemeAtPoint(IllumitationScheme* scheme, UintParameter pointId)
{
	if (tryApplyById(scheme, pointId))
		return;

	// We have no point with this id. Lets try to use zone-wide point
	auto jt = m_killZonesManager.sensorToZone().find(pointId);
	if (jt != m_killZonesManager.sensorToZone().end())
	{
		// We know kill zone id
		if (tryApplyZoneWide(scheme, jt->second))
			return;
	}

	// We cannot find point by id and its zone has no points. Lets try system-wide point
	tryApplySystemWide(scheme);

	// If we here, no points were added
}

void LedVibroManager::applyIlluminationSchemeAtZoneByPointId(IllumitationScheme* scheme, UintParameter pointId)
{
	auto jt = m_killZonesManager.sensorToZone().find(pointId);
	if (jt != m_killZonesManager.sensorToZone().end())
	{
		// If zone found for point
		UintParameter zoneId = jt->second;

		// Lets try to visit all points in zone and apply illumination inside it
		bool wasAnythingDone = false;
		auto visitor = [this, &wasAnythingDone, scheme](UintParameter sensorId)
		{
			if (tryApplyById(scheme, sensorId))
				wasAnythingDone = true;
		};

		m_killZonesManager.visitSensorsInZone(zoneId, visitor);

		if (wasAnythingDone)
			return;

		// No points in zone was visited :(
		if (tryApplyZoneWide(scheme, zoneId))
			return;
	}
	tryApplySystemWide(scheme);
}

bool LedVibroManager::tryApplyById(IllumitationScheme* scheme, UintParameter pointId)
{
	auto it = m_pointsById.find(pointId);
	if (it != m_pointsById.end())
	{
		it->second->applyIlluminationScheme(scheme);
		return true;
	}
	return false;
}

bool LedVibroManager::tryApplyZoneWide(IllumitationScheme* scheme, UintParameter zoneId)
{
	auto it = m_zoneWides.find(zoneId);
	if (it != m_zoneWides.end())
	{
		// We have zone-wide point, it is good
		it->second->applyIlluminationScheme(scheme);
		return true;
	}
	return false;
}

bool LedVibroManager::tryApplySystemWide(IllumitationScheme* scheme)
{
	if (m_systemWide != nullptr)
	{
		m_systemWide->applyIlluminationScheme(scheme);
		return true;
	}
	return false;
}

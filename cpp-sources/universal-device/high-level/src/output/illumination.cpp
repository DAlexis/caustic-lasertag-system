#include "output/illumination.hpp"
#include "core/logging.hpp"

void IllumitationScheme::changeStatesToColor(Color c)
{
	if (!m_autoColor || c == m_currentColor)
		return;

	switch(c)
	{
	case Color::red:
		for (auto &it : tasks)
		{
			it.state.r = it.templ.teamColor;
			it.state.g = it.templ.otherColors;
			it.state.b = it.templ.otherColors;
		}
		break;
	case Color::green:
		for (auto &it : tasks)
		{
			it.state.r = it.templ.otherColors;
			it.state.g = it.templ.teamColor;
			it.state.b = it.templ.otherColors;
		}
		break;
	case Color::blue:
		for (auto &it : tasks)
		{
			it.state.r = it.templ.otherColors;
			it.state.g = it.templ.otherColors;
			it.state.b = it.templ.teamColor;
		}
		break;
	case Color::yellow:
		for (auto &it : tasks)
		{
			it.state.r = it.templ.teamColor;
			it.state.g = it.templ.teamColor;
			it.state.b = it.templ.otherColors;
		}
		break;
	default:
		break;
	}
}

void IllumitationScheme::setAutoColor(bool enabled)
{
	m_autoColor = enabled;
}

LedVibroManager::LedVibroManager(KillZonesManager& mgr) :
	m_killZonesManager(mgr)
{
}

void LedVibroManager::addPoint(IRGBVibroPointPhysical* m_point, bool zoneWide, bool systemWide)
{
	m_pointsById[m_point->getId()] = m_point;

	UintParameter zoneId = 1;
	auto jt = m_killZonesManager.sensorToZone().find(m_point->getId());
	if (jt != m_killZonesManager.sensorToZone().end())
	{
		zoneId = jt->second;
	} else {
		warning << "LedVibroManager::addPoint: point with id " << m_point->getId() << " was not assigned to any zone";
	}

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

void LedVibroManager::applyIlluminationSchemeAllPoints(const IllumitationScheme* scheme)
{
	for(auto it=m_pointsById.begin(); it!=m_pointsById.end(); ++it)
	{
		it->second->applyIlluminationScheme(scheme);
	}
}

void LedVibroManager::applyIlluminationSchemeAtPoint(const IllumitationScheme* scheme, UintParameter pointId)
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

void LedVibroManager::interrogate()
{
	for (auto it : m_pointsById)
		it.second->interrogate();
}

bool LedVibroManager::tryApplyById(const IllumitationScheme* scheme, UintParameter pointId)
{
	auto it = m_pointsById.find(pointId);
	if (it != m_pointsById.end())
	{
		it->second->applyIlluminationScheme(scheme);
		return true;
	}
	return false;
}

bool LedVibroManager::tryApplyZoneWide(const IllumitationScheme* scheme, UintParameter zoneId)
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

bool LedVibroManager::tryApplySystemWide(const IllumitationScheme* scheme)
{
	if (m_systemWide != nullptr)
	{
		m_systemWide->applyIlluminationScheme(scheme);
		return true;
	}
	return false;
}

IlluminationSchemesManager::IlluminationSchemesManager(TeamGameId& id) :
		m_teamId(id)
{
	m_anyCommand.tasks.push_back(IllumitationScheme::Task(  0,   0,   0, 0));
	m_anyCommand.tasks.push_back(IllumitationScheme::Task(255,   0,   0, 100));
	m_anyCommand.tasks.push_back(IllumitationScheme::Task(  0,   0,   0, 200));
	m_anyCommand.tasks.push_back(IllumitationScheme::Task(255,   0,   0, 200));
	m_anyCommand.tasks.push_back(IllumitationScheme::Task(  0,   0,   0, 200));

	m_init.setAutoColor(false);
	m_init.tasks.push_back(IllumitationScheme::Task(  0,   0,   0,   0, 0));
	m_init.tasks.push_back(IllumitationScheme::Task(255,   0,   0,   0, 200));
	m_init.tasks.push_back(IllumitationScheme::Task(  0,   0,  20,   0, 200));
	m_init.tasks.push_back(IllumitationScheme::Task(  0,   0, 255,   0, 200));
	m_init.tasks.push_back(IllumitationScheme::Task( 20,  20,   0,   0, 200));
	m_init.tasks.push_back(IllumitationScheme::Task(255, 255,   0,   0, 200));
	m_init.tasks.push_back(IllumitationScheme::Task(  0,  20,   0,   0, 200));
	m_init.tasks.push_back(IllumitationScheme::Task(  0, 255,   0,   0, 200));
	m_init.tasks.push_back(IllumitationScheme::Task(  0,   0,   0,   0, 200));

	m_death.tasks.push_back(IllumitationScheme::Task(255,   0,   0,   0));
	m_death.tasks.push_back(IllumitationScheme::Task(255,   0,   0,   500));
	m_death.tasks.push_back(IllumitationScheme::Task(  0,   0,   0,   0));
	m_death.tasks.push_back(IllumitationScheme::Task(  0,   0,   0,   500));

	m_death.tasks.push_back(IllumitationScheme::Task(255,   0,   0,   0));
	m_death.tasks.push_back(IllumitationScheme::Task(255,   0,   0,   500));
	m_death.tasks.push_back(IllumitationScheme::Task(  0,   0,   0,   0));
	m_death.tasks.push_back(IllumitationScheme::Task(  0,   0,   0,   500));

	m_death.tasks.push_back(IllumitationScheme::Task(255,   0,   0,   0));
	m_death.tasks.push_back(IllumitationScheme::Task(255,   0,   0,   500));
	m_death.tasks.push_back(IllumitationScheme::Task(  0,   0,   0,   0));
	m_death.tasks.push_back(IllumitationScheme::Task(  0,   0,   0,   500));

	m_death.tasks.push_back(IllumitationScheme::Task(255,   0,   0,   0));
	m_death.tasks.push_back(IllumitationScheme::Task(128,   0,   0,   1000));
	m_death.tasks.push_back(IllumitationScheme::Task( 64,   0,   0,   1000));
	m_death.tasks.push_back(IllumitationScheme::Task( 32,   0,   0,   1000));
	m_death.tasks.push_back(IllumitationScheme::Task( 16,   0,   0,   1000));
	m_death.tasks.push_back(IllumitationScheme::Task(  8,   0,   0,   1000));
	m_death.tasks.push_back(IllumitationScheme::Task(  4,   0,   0,   1000));
	m_death.tasks.push_back(IllumitationScheme::Task(  2,   0,   0,   1000));
	m_death.tasks.push_back(IllumitationScheme::Task(  0,   0,   0,   1000));

	m_wound.tasks.push_back(IllumitationScheme::Task(255,   0,   0,   0));
	m_wound.tasks.push_back(IllumitationScheme::Task(128,   0,   0,   100));
	m_wound.tasks.push_back(IllumitationScheme::Task( 64,   0,   0,   100));
	m_wound.tasks.push_back(IllumitationScheme::Task( 32,   0,   0,   100));
	m_wound.tasks.push_back(IllumitationScheme::Task( 16,   0,   0,   100));
	m_wound.tasks.push_back(IllumitationScheme::Task(  8,   0,   0,   100));
	m_wound.tasks.push_back(IllumitationScheme::Task(  4,   0,   0,   100));
	m_wound.tasks.push_back(IllumitationScheme::Task(  2,   0,   0,   100));
	m_wound.tasks.push_back(IllumitationScheme::Task(  1,   0,   0,   100));
}

IllumitationScheme::Color IlluminationSchemesManager::getCurrentColor()
{
	switch(m_teamId)
	{
	case teamRed:
		return IllumitationScheme::Color::red;
	case teamBlue:
		return IllumitationScheme::Color::blue;
	case teamYellow:
		return IllumitationScheme::Color::yellow;
	case teamGreen:
		return IllumitationScheme::Color::green;
	default:
		return IllumitationScheme::Color::empty;
	}
}

const IllumitationScheme& IlluminationSchemesManager::anyCommand()
{
	m_anyCommand.changeStatesToColor(getCurrentColor());
	return m_anyCommand;
}

const IllumitationScheme& IlluminationSchemesManager::init()
{
	m_init.changeStatesToColor(getCurrentColor());
	return m_init;
}

const IllumitationScheme& IlluminationSchemesManager::death()
{
	m_death.changeStatesToColor(getCurrentColor());
	return m_death;
}

const IllumitationScheme& IlluminationSchemesManager::wound()
{
	m_wound.changeStatesToColor(getCurrentColor());
	return m_wound;
}

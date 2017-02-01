/*
 * illumination.hpp
 *
 *  Created on: 24 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_ILLUMINATION_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_ILLUMINATION_HPP_

#include "sensors/kill-zones-manager.hpp"
#include "utils/interfaces.hpp"

#include <stdint.h>
#include <vector>

class IllumitationScheme
{
public:
	enum class Color {
		red, green, blue, yellow, empty
	};

	struct StateTemplate
	{
		uint8_t teamColor = 0;
		uint8_t otherColors = 0;
	};


	struct State
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t vibro = 0;
	};

	struct Task
	{
		Task(uint8_t teamColor, uint8_t otherColors, uint8_t vibro, uint32_t delay) :
			delayFromPrev(delay)
		{ templ.teamColor = teamColor; templ.otherColors = otherColors; state.vibro = vibro; }
		State state;
		StateTemplate templ;
		uint32_t delayFromPrev; // ms
	};

	void changeStatesToColor(Color c);

	std::vector<Task> tasks;
private:
	Color m_currentColor = Color::empty;
};

class IRGBVibroPointPhysical : public IInterrogatable
{
public:
	virtual ~IRGBVibroPointPhysical() {}
	virtual void applyIlluminationScheme(const IllumitationScheme* scheme) = 0;
	virtual UintParameter getId() = 0;
};

class IlluminationSchemesManager
{
public:
	IlluminationSchemesManager(TeamGameId& id);
	const IllumitationScheme& anyCommand();

private:
	IllumitationScheme::Color getCurrentColor();
	TeamGameId& m_teamId;

	IllumitationScheme m_anyCommand;
};

class LedVibroManager : public IInterrogatable
{
public:
	LedVibroManager(KillZonesManager& mgr);
	void addPoint(IRGBVibroPointPhysical* m_point, UintParameter zoneId = 0, bool zoneWide = false, bool systemWide = false);
	void applyIlluminationSchemeAtPoint(const IllumitationScheme* scheme, UintParameter pointId);
	void applyIlluminationSchemeAtZoneByPointId(IllumitationScheme* scheme, UintParameter pointId);
	void interrogate() override;

private:
	bool tryApplyById(const IllumitationScheme* scheme, UintParameter pointId);
	bool tryApplyZoneWide(const IllumitationScheme* scheme, UintParameter zoneId);
	bool tryApplySystemWide(const IllumitationScheme* scheme);

	KillZonesManager& m_killZonesManager;
	IRGBVibroPointPhysical* m_systemWide = nullptr;
	std::map<UintParameter, IRGBVibroPointPhysical*> m_zoneWides;
	std::map<UintParameter, IRGBVibroPointPhysical*> m_pointsById;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_ILLUMINATION_HPP_ */

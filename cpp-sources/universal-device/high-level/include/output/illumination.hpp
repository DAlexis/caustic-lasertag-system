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

struct IllumitationScheme
{
	struct State
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t vibro = 0;
	};

	struct Task
	{
		Task(uint8_t r, uint8_t g, uint8_t b, uint8_t v, uint32_t delay) :
			delayFromPrev(delay)
		{ state.r = r; state.g = g; state.b = b; state.vibro = v; }
		State state;
		uint32_t delayFromPrev; // ms
	};

	std::vector<Task> tasks;
};

class IRGBVibroPointPhysical : public IInterrogatable
{
public:
	virtual ~IRGBVibroPointPhysical() {}
	virtual void applyIlluminationScheme(IllumitationScheme* scheme) = 0;
	virtual UintParameter getId() = 0;
};

struct DefaultIlluminationSchemes
{
	DefaultIlluminationSchemes();
	IllumitationScheme anyCommand;
};

class LedVibroManager : public IInterrogatable
{
public:
	LedVibroManager(KillZonesManager& mgr);
	void addPoint(IRGBVibroPointPhysical* m_point, UintParameter zoneId = 0, bool zoneWide = false, bool systemWide = false);
	void applyIlluminationSchemeAtPoint(IllumitationScheme* scheme, UintParameter pointId);
	void applyIlluminationSchemeAtZoneByPointId(IllumitationScheme* scheme, UintParameter pointId);
	void interrogate() override;

private:
	bool tryApplyById(IllumitationScheme* scheme, UintParameter pointId);
	bool tryApplyZoneWide(IllumitationScheme* scheme, UintParameter zoneId);
	bool tryApplySystemWide(IllumitationScheme* scheme);

	KillZonesManager& m_killZonesManager;
	IRGBVibroPointPhysical* m_systemWide = nullptr;
	std::map<UintParameter, IRGBVibroPointPhysical*> m_zoneWides;
	std::map<UintParameter, IRGBVibroPointPhysical*> m_pointsById;
};

extern DefaultIlluminationSchemes defaultIlluminationSchemes;

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_OUTPUT_ILLUMINATION_HPP_ */

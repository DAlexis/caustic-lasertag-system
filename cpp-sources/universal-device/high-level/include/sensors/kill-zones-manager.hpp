/*
 * kill-zones-manager.hpp
 *
 *  Created on: 27 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_KILL_ZONES_MANAGER_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_KILL_ZONES_MANAGER_HPP_

#include "rcsp/base-types.hpp"
#include "game/game-base-types.hpp"

#include <map>
#include <functional>

class KillZonesManager
{
public:
	using ZoneVisitor = std::function<void(UintParameter)>;
	void assignSensorToZone(UintParameter sensorId, UintParameter zoneId);
	void setDamageCoefficient(UintParameter zoneId, FloatParameter& damageCoefficient);

	const std::map<UintParameter, UintParameter>& sensorToZone();
	FloatParameter getDamageCoefficient(UintParameter sensorId);
	void visitSensorsInZone(UintParameter zoneId, ZoneVisitor visitor);

private:
	std::map<UintParameter, UintParameter> m_sensorToZone;
	std::map<UintParameter, FloatParameter*> m_damageCoefficient;

};


#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_IR2_KILL_ZONES_MANAGER_HPP_ */

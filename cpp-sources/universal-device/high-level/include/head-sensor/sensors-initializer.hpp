/*
 * sensors-initializer.hpp
 *
 *  Created on: 4 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_SENSORS_INITIALIZER_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_SENSORS_INITIALIZER_HPP_

#include "output/illumination.hpp"
#include "sensors/ir-presentation-receiver.hpp"
#include "sensors/kill-zones-manager.hpp"
#include "hal/io-pins.hpp"

class SensorsInitializer
{
public:
	SensorsInitializer(IRReceiversManager& irMgr, LedVibroManager& lvMgr, KillZonesManager &kzMgr);
	void read(const char* filename);

private:
	constexpr static uint16_t strMaxLen = 50;
	enum class State {
		nothing = 0,
		processingBuiltInSensor,
		processingBuiltInPoint,
		processingZone

	};

	void processKeyValue(const char* group, const char* key, const char* value);

	void finishBuiltInSensorProcessing();
	void finishBuiltInPointProcessing();
	void finishZoneProcessing();

	bool isSensorParamsReady();
	bool isPointParamsReady();
	bool isZoneParamsReady();

	void resetVariables();

	IRReceiversManager& m_irMgr;
	LedVibroManager& m_lvMgr;
	KillZonesManager &m_kzMgr;

	State m_state;

	int32_t m_currentId;
	int m_currentPort;
	int m_currentPin;
	int m_currentTimer;
	bool m_systemWide = false;
	bool m_zoneWide = false;
	float m_damage = 1.0;
	char m_str[strMaxLen];
};




#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_HEAD_SENSOR_SENSORS_INITIALIZER_HPP_ */

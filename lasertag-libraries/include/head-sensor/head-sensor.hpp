/*
 * head-sensor.hpp
 *
 *  Created on: 24 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_

#include "logic/player-config-and-state.hpp"
#include "logic/device.hpp"
#include "head-sensor/kill-zones.hpp"
#include "rcsp/RCSP-modem.hpp"
#include "rcsp/RCSP-state-saver.hpp"
#include "rcsp/broadcast.hpp"
#include "dev/rgb-leds.hpp"
#include "head-sensor/parameters.hpp"

#include <set>

class HeadSensor
{
public:
	HeadSensor();
	void configure(HeadSensorPinoutMapping& pinout);

	PlayerConfiguration playerConfig;
	PlayerState playerState{&playerConfig};
	//DeviceParameters device;

	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerRespawn);
	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerReset);
	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerKill);
	FUNCION_NP(ConfigCodes::AnyDevice::Functions, HeadSensor, resetToDefaults);

	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, registerWeapon, DeviceAddress);
	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, setTeam, uint8_t);

	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, addMaxHealth, int16_t);
private:
	// Test functions
	void testDie(const char*);

	uint8_t getTeamColor();

	void shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage, const float* pZoneModifier = nullptr);
	void dieWeapons();
	void respawnWeapons();
	void turnOffWeapons();

	RGBLeds m_leds;

	//MilesTag2Receiver m_killZone1;

	KillZonesManager m_killZonesManager{
		playerConfig.zone1DamageCoeff,
		playerConfig.zone2DamageCoeff,
		playerConfig.zone3DamageCoeff,
		playerConfig.zone4DamageCoeff,
		playerConfig.zone5DamageCoeff,
		playerConfig.zone6DamageCoeff
	};

};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_ */

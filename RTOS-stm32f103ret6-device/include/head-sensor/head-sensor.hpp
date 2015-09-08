/*
 * head-sensor.hpp
 *
 *  Created on: 24 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_

#include "head-sensor/resources.hpp"
#include "head-sensor/player-config-and-state.hpp"
#include "device/device.hpp"
#include "head-sensor/kill-zones.hpp"
#include "head-sensor/head-sensor-base-types.hpp"
#include "rcsp/operation-codes.hpp"
#include "rcsp/RCSP-state-saver.hpp"
#include "network/broadcast.hpp"
#include "dev/rgb-leds.hpp"
#include "core/device-initializer.hpp"

#include <set>

class HeadSensor : public IAnyDevice
{
public:
	HeadSensor();
	void configure(const Pinout &_pinout);
	void init(const Pinout &pinout);
	void setDafaultPinout(Pinout& pinout);
	bool checkPinout(const Pinout& pinout);

	DeviceConfiguration deviceConfig;
	PlayerConfiguration playerConfig;
	PlayerState playerState{&playerConfig};
	//DeviceParameters device;

	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerRespawn);
	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerReset);
	FUNCION_NP(ConfigCodes::HeadSensor::Functions, HeadSensor, playerKill);
	FUNCION_NP(ConfigCodes::AnyDevice::Functions, HeadSensor, resetToDefaults);

	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, registerWeapon);
	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, setTeam);

	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, addMaxHealth);

	FUNCION_1P(ConfigCodes::HeadSensor::Functions, HeadSensor, notifyIsDamager);      ///< Feedback when player was damaged or killed
private:

	constexpr static uint32_t heartbeatPeriod = 2000000;
	// Test functions
	void testDie(const char*);

	uint8_t getTeamColor();

	void shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage);
	void dieWeapons();
	void respawnWeapons();
	void turnOffWeapons();
	void notifyDamager(PlayerMT2Id player, uint8_t damagerTeam, uint8_t state = 0);
	void weaponWoundAndShock();

	void sendHeartbeat();

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

	Time m_shockDelayBegin = 0;
	TasksPool m_tasksPool;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_ */

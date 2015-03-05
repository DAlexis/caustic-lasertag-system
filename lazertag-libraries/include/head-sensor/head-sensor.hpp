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
#include "rcsp/RCSP-modem.hpp"
#include "dev/miles-tag-2.hpp"

#include <set>

class HeadSensor
{
public:
	HeadSensor();
	void configure();

	PlayerConfiguration playerConfig;
	PlayerState playerState{&playerConfig};
	DeviceParameters device;

	FUNCION_NP(ConfigCodes::Player::Functions, HeadSensor, playerRespawn);
	FUNCION_NP(ConfigCodes::Player::Functions, HeadSensor, playerReset);
	FUNCION_NP(ConfigCodes::Player::Functions, HeadSensor, playerKill);
	FUNCION_1P(ConfigCodes::Player::Functions, HeadSensor, addMaxHealth, int16_t);

private:
	// Test functions
	void testDie(const char*);

	void shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage);
	void turnOnAndResetWeapons();
	void turnOffWeapons();

	MilesTag2Receiver m_mainSensor;
	std::set<DeviceAddress> m_weapons;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_ */

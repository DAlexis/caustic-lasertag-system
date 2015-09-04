/*
 * head-sensor-rifle-communication.hpp
 *
 *  Created on: 10 июня 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_LOGIC_HEAD_SENSOR_RIFLE_COMMUNICATION_HPP_
#define INCLUDE_LOGIC_HEAD_SENSOR_RIFLE_COMMUNICATION_HPP_

#include "dev/MT2-base-types.hpp"

struct DamageNotification
{
	constexpr static uint8_t injured = 0;
	constexpr static uint8_t killed = 1;

	PlayerMT2Id damager = 0;
	uint8_t damagedTeam = 0;
	PlayerMT2Id target = 0;
	uint8_t state = 0;
};

struct NotificationSoundCase
{
	constexpr static uint8_t enemyInjured = 0;
	constexpr static uint8_t enemyKilled = 1;
	constexpr static uint8_t friendInjured = 2;

};



#endif /* INCLUDE_LOGIC_HEAD_SENSOR_RIFLE_COMMUNICATION_HPP_ */

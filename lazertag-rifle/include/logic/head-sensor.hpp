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
#include "logic/package-sender.hpp"
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

private:
	// Test functions
	void testDie(const char*);

	void shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage);
	void respawn();
	void reset();

	MilesTag2Receiver m_mainSensor;
	std::set<DeviceAddress> m_weapons;
};



#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_ */

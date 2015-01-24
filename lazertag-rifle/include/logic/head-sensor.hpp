/*
 * head-sensor.hpp
 *
 *  Created on: 24 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_
#define LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_

#include "logic/player.hpp"
#include "dev/miles-tag-2.hpp"

class HeadSensor
{
public:
	HeadSensor();
	void configure();

	PlayerParameters player;

private:
	void shotCallback(unsigned int teamId, unsigned int playerId, unsigned int damage);

	MilesTag2Receiver m_mainSensor;
};






#endif /* LAZERTAG_RIFLE_INCLUDE_LOGIC_HEAD_SENSOR_HPP_ */

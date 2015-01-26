/*
 * player.cpp
 *
 *  Created on: 25 янв. 2015 г.
 *      Author: alexey
 */

#include "logic/player.hpp"

bool PlayerState::damage(uint8_t damage)
{
	UintParameter totalDamage = damage;
	if (totalDamage > health)
		health = 0;
	else
		health -= totalDamage;
}

bool PlayerState::isAlive()
{
	return health != 0;
}

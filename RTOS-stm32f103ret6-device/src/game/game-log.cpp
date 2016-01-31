/*
 * game-log.cpp
 *
 *  Created on: 28 янв. 2016 г.
 *      Author: alexey
 */

#include "game/game-log.hpp"
#include "string.h"

using namespace GameLog;


Event::Event()
{
	memset(eventParameter, 0, eventParameterSize);
}

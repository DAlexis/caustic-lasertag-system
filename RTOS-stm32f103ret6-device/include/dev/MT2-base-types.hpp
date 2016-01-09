/*
 * MT2-base-types.hpp
 *
 *  Created on: 4 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_DEV_MT2_BASE_TYPES_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_DEV_MT2_BASE_TYPES_HPP_

#include "rcsp/base-types.hpp"

#include <stdint.h>

using PlayerMT2Id = uint8_t;
using TeamMT2Id = uint8_t;

struct ShotMessage {
	ShotMessage(TeamMT2Id _teamId = 0, PlayerMT2Id _playerId = 0, UintParameter _damage = 0) :
		teamId(_teamId), playerId(_playerId), damage(_damage)
	{}
	TeamMT2Id teamId;
	PlayerMT2Id playerId;
	UintParameter damage;
};

#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_DEV_MT2_BASE_TYPES_HPP_ */

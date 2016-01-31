/*
 * game-log.hpp
 *
 *  Created on: 8 нояб. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_GAME_GAME_LOG_HPP_
#define INCLUDE_GAME_GAME_LOG_HPP_

#include "ir/MT2-base-types.hpp"
#include "game/game-base-types.hpp"

#include "hal/system-clock.hpp"

namespace GameLog
{

	class BaseStatsCounter
	{
	public:
		void registerDamage(PlayerMT2Id player);
		void registerKill(PlayerMT2Id player);

	private:

	};

	struct Event
	{
		constexpr static uint16_t eventParameterSize = 4;
		Event();
		uint8_t eventType = 0;
		Time eventTime = 0;
		uint8_t eventParameter[eventParameterSize];
	};


	class PlayerDeathCounter
	{
	public:
		void registerPlayerDeath(TeamMT2Id team, PlayerMT2Id player);

	private:
	};
}





#endif /* INCLUDE_GAME_GAME_LOG_HPP_ */

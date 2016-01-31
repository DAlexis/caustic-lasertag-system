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
#include "rcsp/state-saver-interface.hpp"
#include "hal/system-clock.hpp"
#include "network/network-base-types.hpp"
#include "fatfs.h"

#include <map>

namespace GameLog
{
	class BaseStatsCounter : public IAnyStateSaver
	{
	public:
		void registerDamage(PlayerMT2Id player, uint16_t damage);
		void registerHit(PlayerMT2Id player);
		void registerKill(PlayerMT2Id player);
		void clear();

		void saveState();
		void restoreFromFile();

		void setStatsReceiver(DeviceAddress addr);
		void interrogate();
		void sentStats();

	private:
		static const char* filename;

		enum StatsSendingState {
			S_NOTHING = 0,
			S_IN_PROCESS = 1,
			S_NEED_RESET = 2,
			S_WAIT_FOR_RESTART = 3
		};

		void checkAndCreate(PlayerMT2Id player);

		std::map<PlayerMT2Id, PvPDamageResults> m_results;
		std::map<PlayerMT2Id, PvPDamageResults>::iterator m_sendingIterator;

		FIL m_file;

		DeviceAddress m_statsReceiver;
		StatsSendingState m_sendingState;
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

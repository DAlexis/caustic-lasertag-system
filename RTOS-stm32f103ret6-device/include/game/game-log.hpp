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
#include "core/os-wrappers.hpp"
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
		void sendStats();

	private:
		static const char* filename;
		constexpr static uint32_t delayAfterChunk = 50000;

		enum StatsSendingState {
			S_NOTHING = 0,
			S_READY_TO_TRANSMIT,
			S_WAITING_FOR_TRANSMISSIO_RESULT,
			S_WAIT_DELAY_AFTER_CHUNK
		};

		void sendNextPackage();
		void prepareTransmission();
		void waitDelay();
		void onTransmissionBroken();
		void onTransmissionSucceeded();

		void checkAndCreate(PlayerMT2Id player);

		std::map<PlayerMT2Id, PvPDamageResults> m_results;
		std::map<PlayerMT2Id, PvPDamageResults>::iterator m_sendingIterator;
		bool m_iteratorCorrupted = false;

		FIL m_file;

		DeviceAddress m_statsReceiver;
		StatsSendingState m_sendingState;

		CritialSection m_iteratorCheck;
		Time m_waitingBeginned = 0;
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

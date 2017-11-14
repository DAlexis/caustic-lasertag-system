/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/


#ifndef INCLUDE_GAME_GAME_LOG_HPP_
#define INCLUDE_GAME_GAME_LOG_HPP_

#include "game/game-base-types.hpp"
#include "rcsp/state-saver-interface.hpp"
#include "hal/system-clock.hpp"
#include "network/network-base-types.hpp"
#include "network/broadcast.hpp"
#include "network/network-client.hpp"
#include "core/os-wrappers.hpp"
#include "fatfs.h"

#include <map>

namespace GameLog
{
	class BaseStatsCounter : public IAnyStateSaver
	{
	public:
		BaseStatsCounter(const PlayerGameId& hostPlayerId, INetworkClientSender& networkClientSender);
		void registerDamage(PlayerGameId player, uint16_t damage);
		void registerHit(PlayerGameId player);
		void registerKill(PlayerGameId player);
		void clear();

		void saveState() override;
		void restoreFromFile();

		void interrogate();
		void sendStats(DeviceAddress target);

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

		void checkAndCreate(PlayerGameId enemy);

		std::map<PlayerGameId, PvPDamageResults> m_results;
		std::map<PlayerGameId, PvPDamageResults>::iterator m_sendingIterator;
		bool m_iteratorCorrupted = false;

		FIL m_file;

		DeviceAddress m_statsReceiver;
		StatsSendingState m_sendingState = S_NOTHING;

		CritialSection m_iteratorCheck;
		Time m_waitingBeginned = 0;

		const PlayerGameId& m_hostPlayerId;
        INetworkClientSender& m_networkClientSender;
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
		void registerPlayerDeath(TeamGameId team, PlayerGameId player);

	private:
	};
}





#endif /* INCLUDE_GAME_GAME_LOG_HPP_ */

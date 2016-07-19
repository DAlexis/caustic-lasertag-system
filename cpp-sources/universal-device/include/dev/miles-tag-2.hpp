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

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_

#include "ir/MT2-base-types.hpp"
#include "hal/fire-emitter.hpp"
#include "hal/io-pins.hpp"
#include "core/os-wrappers.hpp"
#include "core/device-initializer.hpp"
#include <functional>
#include <stdint.h>

#define MILESTAG2_MAX_MESSAGE_LENGTH 	40

// unsigned int teamId, unsigned int playerId, unsigned int damage
using MilesTag2ShotCallbackISR = std::function<void(unsigned int /*teamId*/, unsigned int /*playerId*/, unsigned int /*damage*/)>;

class MilesTag2Transmitter
{
public:
	MilesTag2Transmitter(const UintParameter& power);
	~MilesTag2Transmitter() {}

	void init(const Pinout& pinout, unsigned int fireEmitterNumber = 0);
	void setPlayerIdReference(PlayerMT2Id& playerId);
	void setTeamIdReference(TeamMT2Id& teamId);

	PlayerMT2Id getPlayerId() { return *m_playerId; }
	uint8_t getTeamId() { return *m_teamId; }

	// Standard commands
	void shot(uint8_t damage);
	void addHealth(int16_t value);
	void setTeam(TeamMT2Id teamId);
	void addRounds(uint8_t value);
	void adminKill();
	void pauseOrUnpause();
	void startGame();
	void restoreDefaults();
	void respawn();
	void newGameImmediate();
	void fullAmmo();
	void endGame();
	void resetClock();
	void initializePlayer();
	void explodePlayer();
	void newGameReady();
	void fullHealth();
	void fullArmor();
	void clearScores();
	void testSensors();
	void stunPlayer();
	void disarmPlayer();

	void ammoPickup(uint8_t ammoBoxId);     // 0x00-0x0F
	void healthPickup(uint8_t healthBoxId); // 0x00-0x0F
	void flagPickup(uint8_t flagBoxId);     // 0x00-0x0F

private:
	bool nextBit();
	void cursorToStart();
	void fireCallback(bool wasOnState);
	void beginTransmission();

	uint8_t *m_pCurrentByte;
	uint8_t m_currentBit;
	uint8_t m_data[MILESTAG2_MAX_MESSAGE_LENGTH];

	unsigned int m_currentLength;

	unsigned int m_length;

	void sendCommand(uint8_t commandCode);
	uint8_t encodeDamage(uint8_t damage);
	PlayerMT2Id m_playerIdStub = 1;
	PlayerMT2Id *m_playerId = &m_playerIdStub;

	uint8_t m_teamIdStub = 0;
	uint8_t *m_teamId = &m_teamIdStub;

	bool m_sendingHeader = false;
	IFireEmitter *m_fireEmitter = nullptr;

	const UintParameter& m_power;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_ */

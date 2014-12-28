/*
 * miles-tag-2.hpp
 *
 *  Created on: 28 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_

#include "hal/fire-emitter.hpp"

#include <functional>
#include <stdint.h>

#define MILESTAG2_MAX_MESSAGE_LENGTH 	40

// unsigned int teamId, unsigned int playerId, unsigned int damage
using MilesTag2ShotCallback = std::function<void(unsigned int /*teamId*/, unsigned int /*playerId*/, unsigned int /*damage*/)>;

class MilesTag2Transmitter
{
public:
	MilesTag2Transmitter();
	~MilesTag2Transmitter() {}

	void init();
	void setPlayerId(uint8_t playerId);
	void setTeamId(uint8_t teamId);

	// Standard commands
	void shot(uint8_t damage);
	void addHealth(uint8_t value);
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
	uint8_t m_playerId = 1;
	uint8_t m_teamId = 0;
	bool m_sendingHeader;
	IFireEmitter *m_fireEmitter = nullptr;
};

/*
class MilesTag2Receiver
{
public:
    ~MilesTag2Receiver() {}
    void setShortMessageCallback(MilesTag2ShotCallback callback, void* object);
    void interrogate();
    void init(unsigned int channel);
    void interruptionHandler();

    void enableDebug(bool debug);
private:
};*/

#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_ */

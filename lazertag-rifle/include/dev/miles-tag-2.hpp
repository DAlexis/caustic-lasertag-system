/*
 * miles-tag-2.hpp
 *
 *  Created on: 28 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_

#include "hal/fire-emitter.hpp"
#include "hal/ext-interrupts.hpp"

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

	uint8_t getPlayerId() { return m_playerId; }
	uint8_t getTeamId() { return m_teamId; }

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


class MilesTag2Receiver
{
public:
    ~MilesTag2Receiver() {}
    void setShortMessageCallback(MilesTag2ShotCallback callback);
    void init(IExternalInterruptManager* exti);
    void turnOn();
    void turnOff();
    void interrogate();

    void enableDebug(bool debug);
private:

	enum ReceivingState
	{
		RS_WAITING_HEADER = 0,
		RS_HEADER_BEGINNED = 1,
		RS_SPACE = 2,
		RS_BIT = 3
	};

    uint8_t decodeDamage(uint8_t damage);
	void saveBit(bool value);
	bool isCorrect(unsigned int value, unsigned int min, unsigned int max);
	int getCurrentLength();
	bool getBit(unsigned int n);
	void parseAndCallShot();
	void resetReceiver();
	void interruptHandler(bool state);

	ReceivingState m_state = RS_WAITING_HEADER;
	bool m_falseImpulse = false;

	MilesTag2ShotCallback m_shotCallback = nullptr;
	unsigned int m_lastTime = 0;
	bool m_dataReady = false;
	bool m_debug = false;

	uint8_t m_data[MILESTAG2_MAX_MESSAGE_LENGTH];
	uint8_t *m_pCurrentByte = m_data;
	uint8_t m_currentBit;
    IExternalInterruptManager* m_exti = nullptr;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_ */

/*
 * miles-tag-2.hpp
 *
 *  Created on: 28 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_
#define LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_

#include "dev/MT2-base-types.hpp"
#include "hal/fire-emitter.hpp"
#include "hal/io-pins.hpp"
#include "core/os-wrappers.hpp"
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

	void init(unsigned int fireEmitterNumber = 0);
	void setPlayerIdReference(PlayerMT2Id& playerId);
	void setTeamIdReference(TeamMT2Id& teamId);

	/// Set output power in percents. This function may conflict with setChannel()
	void setPower(unsigned int percent);
	/// Set output channel if supported by fire emitter. This function may conflict with setPower()
	void setChannel(unsigned int channel);
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
};


class MilesTag2Receiver
{
public:
	MilesTag2Receiver();
    ~MilesTag2Receiver() {}
    void setShortMessageCallback(MilesTag2ShotCallback callback);
    void init(IIOPin* input);
    void turnOn();
    void turnOff();
    void interrogate();

    void enableDebug(bool debug = true);
private:
    using OnNextInterrogationCallback = std::function<void(void)>;
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
	bool parseConstantSizeMessage();
	void resetReceiver();
	void interruptHandler(bool state);
	/// Currently means "parse general-purpose RCSP message"
	bool parseVariableSizeMessage();

	ReceivingState m_state = RS_WAITING_HEADER;
	bool m_falseImpulse = false;

	MilesTag2ShotCallback m_shotCallback = nullptr;
	unsigned int m_lastTime = 0;
	bool m_dataReady = false;
	bool m_debug = false;

	uint8_t m_data[MILESTAG2_MAX_MESSAGE_LENGTH];
	uint8_t *m_pCurrentByte = m_data;
	uint8_t m_currentBit;
	uint32_t m_dtime = 0;
	uint32_t m_lastDtime = 0;
	IIOPin* m_input = nullptr;
    //IExternalInterruptManager* m_exti = nullptr;
    OnNextInterrogationCallback m_nextInterrogationCallback = nullptr;
    TaskDeferredFromISR m_delayedTask;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_ */

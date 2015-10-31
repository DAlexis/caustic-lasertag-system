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

/**
 * Behavior of receiver:
 *   - in case of shot MilesTag2ShotCallbackISR will be called immediately from ISR
 *   - in case of other message callback for message processing will be stored and then
 *       returned by function getMessageCallback() on next interrogation. No message queue is
 *       implemented now
 */
class MilesTag2Receiver
{
public:
	constexpr static uint16_t rcspMessageMaxBufferSize = 50;
	using ProcessMessageCallback = std::function<void(void)>;

	struct MTMessageContext {
		uint8_t m_rcspMsgBuffer[rcspMessageMaxBufferSize];
		uint8_t size = 0;
		ProcessMessageCallback callback = nullptr;
	};

	MilesTag2Receiver();
    ~MilesTag2Receiver() {}
    void setShortMessageCallbackISR(MilesTag2ShotCallbackISR callback);
    void init(IIOPin* input);
    void turnOn();
    void turnOff();
    void setMessageContext(MTMessageContext& buffer);

    void interrogate();

    void enableDebug(bool debug = true);

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

	/**
	 * Get received message length for this moment
	 * @return Current received length in bits
	 */
	int getCurrentLength();
	bool getBit(unsigned int n);
	bool parseConstantSizeMessageISR();

	void parseShotISR();
	void parseSetTeamISR();

	void resetReceiver();
	void interruptHandler(bool state);

	/**
	 * This function parse messages that may have variable size. Trigger for this is
	 * delay after last received bit
	 * @return
	 */
	bool parseVariableSizeMessage();
	void callGenericRCSPDeserializetion();

	ReceivingState m_state = RS_WAITING_HEADER;
	bool m_falseImpulse = false;

	MilesTag2ShotCallbackISR m_shotCallback = nullptr;
	unsigned int m_lastTime = 0;
	bool m_dataReady = false;
	bool m_debug = false;

	uint8_t m_data[MILESTAG2_MAX_MESSAGE_LENGTH];
	uint8_t *m_pCurrentByte = m_data;
	uint8_t m_currentBit;
	uint32_t m_dtime = 0;
	uint32_t m_lastDtime = 0;
	IIOPin* m_input = nullptr;
	MTMessageContext* m_msgContext = nullptr;
};

#endif /* LAZERTAG_RIFLE_INCLUDE_DEV_MILES_TAG_2_HPP_ */

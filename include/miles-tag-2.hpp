/*
 * miles-tag-2.hpp
 *
 *  Created on: 05 нояб. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_HPP_
#define LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_HPP_

#include <stdint.h>

#define MILESTAG2_MAX_MESSAGE_LENGTH    40

using MilesTag2ShotCallback = void (*) (void* object, unsigned int teamId, unsigned int playerId, unsigned int damage);

class MilesTag2Transmitter
{
public:
    MilesTag2Transmitter();
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
    void sendCommand(uint8_t commandCode);
    void beginTransmission();
    uint8_t encodeDamage(uint8_t damage);
    static void fireCallback(void* object, bool wasOnState);
    bool nextBit();
    uint8_t *m_pCurrentByte;
    uint8_t m_currentBit;

    uint8_t m_data[MILESTAG2_MAX_MESSAGE_LENGTH];
    uint8_t m_playerId;
    uint8_t m_teamId;

    unsigned int m_length;
    unsigned int m_currentLength;

    bool m_sendingHeader;
};



class MilesTag2Receiver
{
public:
    MilesTag2Receiver();
    void setShortMessageCallback(MilesTag2ShotCallback callback, void* object);
    void interrogate();
    void init(unsigned int channel);
    void interruptionHandler();

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
    void resetReceiverBuffer();
    bool isCorrect(unsigned int value, unsigned int min, unsigned int max);
    int getCurrentLength();

    bool getBit(unsigned int n);
    void parseAndCallShot();
    unsigned int m_channel;
    ReceivingState m_state;
    unsigned int m_lastTime;
    bool m_falseImpulse;
    bool m_dataReady;
    bool m_debug;

    MilesTag2ShotCallback m_shotCallback;
    void* m_shotObject;

    uint8_t m_data[MILESTAG2_MAX_MESSAGE_LENGTH];
    uint8_t *m_pCurrentByte;
    uint8_t m_currentBit;
    /*
    static void fireCallback(void* object, bool wasOnState);
    bool nextBit();
    uint8_t *m_pCurrentByte;
    uint8_t m_currentBit;


    uint8_t m_playerId;
    uint8_t m_teamId;

    unsigned int m_length;
    unsigned int m_currentLength;

    bool m_sendingHeader;*/


};


extern MilesTag2Transmitter milesTag2;
extern MilesTag2Receiver milesTag2Receiver;



#endif /* LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_HPP_ */

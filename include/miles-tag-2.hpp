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

enum MilesTag2Action
{
    MT2_DAMAGE,
    MT2_ADD_HEALTH,
    MT2_ADD_ROUNDS,
    /// @todo to be filled
};

using MilesTag2ShotCallback = void (*) (void* object, unsigned int teamId, unsigned int playerId, unsigned int damage);


class MilesTag2Transmitter
{
public:
    MilesTag2Transmitter();
    void init();
    void setPlayerId(uint8_t playerId);
    void setTeamId(uint8_t teamId);
    void shot(uint8_t damage);

private:
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

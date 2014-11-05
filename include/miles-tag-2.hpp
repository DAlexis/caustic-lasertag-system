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

extern MilesTag2Transmitter milesTag2;




#endif /* LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_HPP_ */

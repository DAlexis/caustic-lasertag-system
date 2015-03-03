/*
 * miles-tag-2-timings.h
 *
 *  Created on: 06 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_TIMINGS_H_
#define LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_TIMINGS_H_

#include <stdint.h>
// Transmitter constants
#define HADER_PERIOD            2400
#define BIT_ONE_PERIOD          1200
#define BIT_ZERO_PERIOD         600
#define BIT_WAIT_PERIOD         600

// Receiver constants
#define HEADER_PERIOD_MIN       2300
#define HEADER_PERIOD_MAX       2600

#define BIT_ONE_PERIOD_MIN      1100
#define BIT_ONE_PERIOD_MAX      1500

#define BIT_ZERO_PERIOD_MIN     500
#define BIT_ZERO_PERIOD_MAX     750

#define BIT_WAIT_PERIOD_MIN     500
#define BIT_WAIT_PERIOD_MAX     900

#define TIME_BEFORE_PARSING     HADER_PERIOD


#define SHOT_LENGTH             14
#define MESSAGE_LENGTH          (3*8)

#define MESSAGE_END_BYTE        0xE8
#define RCSP_MESSAGE            0x8D // Not standard Miles Tag 2 message

namespace MT2Std
{
	namespace Byte1
	{
		constexpr uint8_t addHealth = 0x80;
		constexpr uint8_t addRounds = 0x81;
		constexpr uint8_t ammoPickup = 0x8A;
		constexpr uint8_t healthPickup = 0x8B;
		constexpr uint8_t flagPickup = 0x8C;

		constexpr uint8_t command = 0x83;
		constexpr uint8_t RCSPMessage = 0x8D;
	}

	namespace Commands
	{
		constexpr uint8_t adminKill = 0x00;
		constexpr uint8_t pauseOrUnpause = 0x01;
		constexpr uint8_t startGame = 0x02;
		constexpr uint8_t restoreDefaults = 0x03;
		constexpr uint8_t respawn = 0x04;
		constexpr uint8_t newGameImmediate = 0x05;
		constexpr uint8_t fullAmmo = 0x06;
		constexpr uint8_t endGame = 0x07;
		constexpr uint8_t resetClock = 0x08;
		constexpr uint8_t initializePlayer = 0x0A;
		constexpr uint8_t explodePlayer = 0x0B;
		constexpr uint8_t newGameReady = 0x0C;
		constexpr uint8_t fullHealth = 0x0D;
		constexpr uint8_t fullArmor = 0x0F;
		constexpr uint8_t clearScores = 0x14;
		constexpr uint8_t testSensors = 0x15;
		constexpr uint8_t stunPlayer = 0x16;
		constexpr uint8_t disarmPlayer = 0x17;
	}
}

inline int decodeAddHealth(unsigned int code)
{
	return code <= 50 ? code : code - 100;
}

#endif /* LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_TIMINGS_H_ */

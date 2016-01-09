/*
 * miles-tag-2-timings.h
 *
 *  Created on: 06 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_TIMINGS_H_
#define LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_TIMINGS_H_

#include <stdint.h>

namespace MT2Extended
{
	constexpr uint8_t shotLength = 14;
	constexpr uint8_t messageLength = 3*8;
	constexpr uint8_t commandLength = 3*8;

	namespace Byte1
	{
		constexpr uint8_t shotMask  = 0b01111111;
		constexpr uint8_t addHealth = 0x80;
		constexpr uint8_t addRounds = 0x81;
		constexpr uint8_t setTeam   = 0x82; ///< This code marked as RESERVED in original MT2

		constexpr uint8_t ammoPickup = 0x8A;
		constexpr uint8_t healthPickup = 0x8B;
		constexpr uint8_t flagPickup = 0x8C;

		constexpr uint8_t command = 0x83;
		constexpr uint8_t RCSPMessage = 0x8D;
	}

	namespace Byte3
	{
		constexpr uint8_t messageEnd = 0xE8;
		constexpr uint8_t commandEnd = 0xE8;
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

	inline int decodeAddHealth(unsigned int code)
	{
		return code <= 50 ? code : code - 100;
	}

	inline unsigned int encodeAddHealth(int health)
	{
		return health >= 0 ? health : 100 + health;
	}
}




#endif /* LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_TIMINGS_H_ */

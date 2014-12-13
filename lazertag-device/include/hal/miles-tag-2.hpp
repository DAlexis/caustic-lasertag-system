/*
 * miles-tag-2.hpp
 *
 *  Created on: 05 нояб. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_HPP_
#define LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_HPP_

#include <stdint.h>

using MilesTag2ShotCallback = void (*) (void* object, unsigned int teamId, unsigned int playerId, unsigned int damage);

class IMilesTag2Transmitter
{
public:
	virtual ~IMilesTag2Transmitter() {}

	virtual void init() = 0;
	virtual void setPlayerId(uint8_t playerId) = 0;
	virtual void setTeamId(uint8_t teamId) = 0;

	// Standard commands
	virtual void shot(uint8_t damage) = 0;
	virtual void addHealth(uint8_t value) = 0;
	virtual void addRounds(uint8_t value) = 0;
	virtual void adminKill() = 0;
	virtual void pauseOrUnpause() = 0;
	virtual void startGame() = 0;
	virtual void restoreDefaults() = 0;
	virtual void respawn() = 0;
	virtual void newGameImmediate() = 0;
	virtual void fullAmmo() = 0;
	virtual void endGame() = 0;
	virtual void resetClock() = 0;
	virtual void initializePlayer() = 0;
	virtual void explodePlayer() = 0;
	virtual void newGameReady() = 0;
	virtual void fullHealth() = 0;
	virtual void fullArmor() = 0;
	virtual void clearScores() = 0;
	virtual void testSensors() = 0;
	virtual void stunPlayer() = 0;
	virtual void disarmPlayer() = 0;

	virtual void ammoPickup(uint8_t ammoBoxId) = 0;     // 0x00-0x0F
	virtual void healthPickup(uint8_t healthBoxId) = 0; // 0x00-0x0F
	virtual void flagPickup(uint8_t flagBoxId) = 0;     // 0x00-0x0F
};

class IMilesTag2Receiver
{
public:
    virtual ~IMilesTag2Receiver() {}
    virtual void setShortMessageCallback(MilesTag2ShotCallback callback, void* object) = 0;
    virtual void interrogate() = 0;
    virtual void init(unsigned int channel) = 0;
    virtual void interruptionHandler() = 0;

    virtual void enableDebug(bool debug) = 0;
};

extern IMilesTag2Transmitter* milesTag2;
extern IMilesTag2Receiver* milesTag2Receiver;


#endif /* LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_HPP_ */

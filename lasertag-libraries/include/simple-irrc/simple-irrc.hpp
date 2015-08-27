/*
 * simple-irrc.hpp
 *
 *  Created on: 23 февр. 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_SIMPLE_IRRC_SIMPLE_IRRC_HPP_
#define INCLUDE_SIMPLE_IRRC_SIMPLE_IRRC_HPP_

#include "core/scheduler.hpp"
#include "dev/buttons.hpp"
#include "dev/miles-tag-2.hpp"

class SimpleIRRC
{
public:

	constexpr static uint32_t buttonsRepeatPeriod = 200000;
	constexpr static uint32_t buttonsInterrogationPeriod = 10000;
	void init();
	void interrogate();

	/////////
	// Buttons mapping
	// Default
	uint8_t respawnButtonPort    = 0;
	uint8_t respawnButtonPin     = 2;

	uint8_t killButtonPort       = 0;
	uint8_t killButtonPin        = 3;

	uint8_t teamRedButtonPort    = 0;
	uint8_t teamRedButtonPin     = 4;

	uint8_t teamBlueButtonPort   = 0;
	uint8_t teamBlueButtonPin    = 5;

	uint8_t decreaseHPButtonPort = 0;
	uint8_t decreaseHPButtonPin  = 6;

	uint8_t increaseHPButtonPort = 0;
	uint8_t increaseHPButtonPin  = 7;

	uint8_t resetToDefaultButtonPort = 1;
	uint8_t resetToDefaultButtonPin  = 0;

private:
	ButtonManager* m_respawnButton = nullptr;
	void respawnButonCb(bool);

	ButtonManager* m_killButton = nullptr;
	void killButtonCb(bool);

	ButtonManager* m_teamRedButton = nullptr;
	void teamRedButtonCb(bool);

	ButtonManager* m_teamBlueButton = nullptr;
	void teamBlueButtonCb(bool);

	ButtonManager* m_decreaseHPButton = nullptr;
	void decreaseHPButtonCb(bool);

	ButtonManager* m_increaseHPButton = nullptr;
	void increaseHPButtonCb(bool);

	ButtonManager* m_resetToDefaultButton = nullptr;
	void resetToDefaultButtonCb(bool);

	MilesTag2Transmitter m_mt2;
};

#endif /* INCLUDE_SIMPLE_IRRC_SIMPLE_IRRC_HPP_ */
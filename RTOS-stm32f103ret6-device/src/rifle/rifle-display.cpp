/*
 * rifle-display.cpp
 *
 *  Created on: 4 апр. 2016 г.
 *      Author: alexey
 */

#include "rifle/rifle-display.hpp"
#include <stdio.h>

RifleLCD5110Display::RifleLCD5110Display(const RifleOwnerConfiguration *owner, const RifleState *state, const PlayerPartialState *playerState) :
	m_owner(owner),
	m_state(state),
	m_playerState(playerState)
{
}

void RifleLCD5110Display::init()
{
	LCD5110Controller::LcdIO lcdio;
	lcdio.spi = SPIs->getSPI(3);
	lcdio.dataCommand = IOPins->getIOPin(1, 9);
	lcdio.reset = IOPins->getIOPin(1, 10);
	lcdio.chipEnable = IOPins->getIOPin(1, 6);

	m_lcd.init(lcdio);

}

void RifleLCD5110Display::update()
{
	m_lcd.configureSPI();
	m_lcd.clearBuffer();
	m_lcd.setFont(LCD5110Controller::fontStandardAscii5x7);
	m_lcd.stringXY(10, 0, "Caustic LTS");

	if (m_state->isHSConnected)
	{
		m_lcd.setFont(LCD5110Controller::fontStandardAscii5x7);
		char buffer[15];
		sprintf(buffer, "H: %u/%u", m_playerState->healthCurrent, m_playerState->healthMax);
		m_lcd.stringXY(0, 2, buffer);
		sprintf(buffer, "A: %u, M: %u", m_state->bulletsInMagazineCurrent, m_state->magazinesCountCurrent);
		m_lcd.stringXY(0, 3, buffer);
	} else {
		m_lcd.setFont(LCD5110Controller::fontStandardAscii5x7);
		m_lcd.stringXY(0, 2, "Head sensor");
		m_lcd.stringXY(0, 3, "not connected");
	}

	m_lcd.write();
}

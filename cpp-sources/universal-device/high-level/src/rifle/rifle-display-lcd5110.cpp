/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System. 
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/


#include "rifle/rifle-display-lcd5110.hpp"
#include "core/pinout.hpp"
#include <stdio.h>

RifleLCD5110Display::RifleLCD5110Display()
{
}

bool RifleLCD5110Display::init()
{
	LCD5110Controller::LcdIO lcdio;
	lcdio.spi = SPIs->getSPI(UniversalConnectorPinout::instance().SPIindex);
	lcdio.dataCommand = IOPins->getIOPin(
		UniversalConnectorPinout::instance().lcd5110.D_C.port,
		UniversalConnectorPinout::instance().lcd5110.D_C.pin
	);
	lcdio.reset = IOPins->getIOPin(
		UniversalConnectorPinout::instance().lcd5110.RST.port,
		UniversalConnectorPinout::instance().lcd5110.RST.pin
	);
	lcdio.chipEnable = IOPins->getIOPin(
		UniversalConnectorPinout::instance().lcd5110.CE.port,
		UniversalConnectorPinout::instance().lcd5110.CE.pin
	);

	m_lcd.init(lcdio);
	return true;

}

void RifleLCD5110Display::update()
{
	m_lcd.configureSPI();
	m_lcd.clearBuffer();
	m_lcd.setFont(LCD5110Controller::fontStandardAscii5x7);
	m_lcd.stringXY(10, 0, "Caustic LTS");

	char buffer[20];

	if (m_state->rfidState == RifleState::RFIDSate::programHSAddr)
	{
		m_lcd.setFont(LCD5110Controller::fontStandardAscii5x7);
		m_lcd.stringXY(0, 2, "Bring the RFID");
		m_lcd.stringXY(0, 3, "to program HS");
		sprintf(buffer, "to %d:%d:%d",
				(int) m_state->config().headSensorAddr.address[0],
				(int) m_state->config().headSensorAddr.address[1],
				(int) m_state->config().headSensorAddr.address[2]
		);
		m_lcd.stringXY(0, 4, buffer);
	} else if (m_state->rfidState == RifleState::RFIDSate::programMasterCard)
	{
		m_lcd.stringXY(0, 2, "Bring the RFID");
		m_lcd.stringXY(0, 3, "to program");
		m_lcd.stringXY(0, 4, "master card");
	} else if (m_state->isHSConnected)
	{
		m_lcd.setFont(LCD5110Controller::fontStandardAscii5x7);
		sprintf(buffer, "H: %u/%u", m_playerState->healthCurrent, m_playerState->healthMax);
		m_lcd.stringXY(0, 2, buffer);
		sprintf(buffer, "A: %u, M: %u", m_state->bulletsInMagazineCurrent, m_state->magazinesCountCurrent);
		m_lcd.stringXY(0, 3, buffer);
		sprintf(buffer, "Team: %d", (int)m_owner->teamId);
		m_lcd.stringXY(0, 4, buffer);
		sprintf(buffer, "Player: %d", (int)m_owner->playerId);
		m_lcd.stringXY(0, 5, buffer);
	} else {
		m_lcd.stringXY(0, 2, "Head sensor");
		sprintf(buffer, " by %d:%d:%d",
				(int) m_state->config().headSensorAddr.address[0],
				(int) m_state->config().headSensorAddr.address[1],
				(int) m_state->config().headSensorAddr.address[2]
		);
		m_lcd.stringXY(0, 3, buffer);
		m_lcd.stringXY(0, 4, "not connected");
	}

	m_lcd.write();
}

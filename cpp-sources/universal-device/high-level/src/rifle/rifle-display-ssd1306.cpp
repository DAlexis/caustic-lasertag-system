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

#include "rifle/rifle-display-ssd1306.hpp"
#include "hal/i2c.hpp"

bool RifleSSD1306Display::init()
{
    return m_display.init(I2Cs->get(2));
}

void RifleSSD1306Display::update()
{
    m_display.Fill(SSD1306Display::Color::black);
    m_display.GotoXY(1, 1);
    m_display.Puts("Caustic LTS", &Font_7x10, SSD1306Display::Color::white);

    char buffer[30];

    if (m_state->rfidState == RifleState::RFIDSate::programHSAddr)
    {
    	m_display.GotoXY(1, 24);
		m_display.Puts("Bring the RFID", &Font_7x10, SSD1306Display::Color::white);
		m_display.GotoXY(1, 36);
		m_display.Puts("to program HS", &Font_7x10, SSD1306Display::Color::white);
		m_display.GotoXY(1, 48);
		sprintf(buffer, "to %d:%d:%d",
				(int) m_state->config().headSensorAddr.address[0],
				(int) m_state->config().headSensorAddr.address[1],
				(int) m_state->config().headSensorAddr.address[2]
		);
		m_display.Puts(buffer, &Font_7x10, SSD1306Display::Color::white);
    } else if (m_state->rfidState == RifleState::RFIDSate::programMasterCard)
    {
    	m_display.GotoXY(1, 24);
    	m_display.Puts("Bring the RFID", &Font_7x10, SSD1306Display::Color::white);
		m_display.GotoXY(1, 36);
		m_display.Puts("to program", &Font_7x10, SSD1306Display::Color::white);
		m_display.GotoXY(1, 48);
		m_display.Puts("master card", &Font_7x10, SSD1306Display::Color::white);
    } else if (m_state->isHSConnected)
    {
        m_display.GotoXY(1, 24);
        sprintf(buffer, "H: %u/%u", m_playerState->healthCurrent, m_playerState->healthMax);
        m_display.Puts(buffer, &Font_7x10, SSD1306Display::Color::white);

        m_display.GotoXY(1, 36);
        sprintf(buffer, "A: %u, M: %u", m_state->bulletsInMagazineCurrent, m_state->magazinesCountCurrent);
        m_display.Puts(buffer, &Font_7x10, SSD1306Display::Color::white);

        m_display.GotoXY(1, 48);
        sprintf(buffer, "Team: %d  id: %d", (int)m_owner->teamId, (int)m_owner->playerId);
        m_display.Puts(buffer, &Font_7x10, SSD1306Display::Color::white);
    } else {
        m_display.GotoXY(1, 12);
        m_display.Puts("Head sensor", &Font_7x10, SSD1306Display::Color::white);
        m_display.GotoXY(1, 24);
        sprintf(buffer, " by %d:%d:%d",
        		(int) m_state->config().headSensorAddr.address[0],
				(int) m_state->config().headSensorAddr.address[1],
				(int) m_state->config().headSensorAddr.address[2]
		);
        m_display.Puts(buffer, &Font_7x10, SSD1306Display::Color::white);
        m_display.GotoXY(1, 36);
        m_display.Puts("not connected", &Font_7x10, SSD1306Display::Color::white);
        //m_display.Fill(SSD1306Display::Color::white);
    }

    m_display.updateScreen();
}


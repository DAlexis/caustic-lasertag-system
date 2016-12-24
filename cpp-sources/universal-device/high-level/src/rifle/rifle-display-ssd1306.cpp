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
    m_display.GotoXY(0, 0);
    m_display.Puts("Caustic LTS", &Font_7x10, SSD1306Display::Color::white);

    if (m_state->isHSConnected)
    {
        char buffer[15];
        m_display.GotoXY(0, 24);
        sprintf(buffer, "H: %u/%u", m_playerState->healthCurrent, m_playerState->healthMax);
        m_display.Puts(buffer, &Font_7x10, SSD1306Display::Color::white);

        m_display.GotoXY(0, 36);
        sprintf(buffer, "A: %u, M: %u", m_state->bulletsInMagazineCurrent, m_state->magazinesCountCurrent);
        m_display.Puts(buffer, &Font_7x10, SSD1306Display::Color::white);

        m_display.GotoXY(0, 48);
        sprintf(buffer, "Team: %d", (int)m_owner->teamId);
        m_display.Puts(buffer, &Font_7x10, SSD1306Display::Color::white);

        m_display.GotoXY(0, 60);
        sprintf(buffer, "Player: %d", (int)m_owner->playerId);
        m_display.Puts(buffer, &Font_7x10, SSD1306Display::Color::white);

    } else {
        m_display.GotoXY(0, 24);
        m_display.Puts("Head sensor", &Font_7x10, SSD1306Display::Color::white);
        m_display.GotoXY(0, 40);
        m_display.Puts("not connected", &Font_7x10, SSD1306Display::Color::white);
    }
    m_display.updateScreen();
}


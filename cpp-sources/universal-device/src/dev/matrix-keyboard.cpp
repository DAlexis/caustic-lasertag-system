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


#include "dev/matrix-keyboard.hpp"
#include "core/logging.hpp"

void MatrixKeyboard::init(uint8_t rowCount, uint8_t colCount, IIOPin** row, IIOPin** col)
{
	m_rowCount = rowCount;
	m_colCount = colCount;
	m_row = row;
	m_col = col;
	for (uint8_t i=0; i< m_rowCount; ++i)
	{
		m_row[i]->switchToOutput();
		m_row[i]->set();
	}

	for (uint8_t i=0; i< m_colCount; ++i)
	{
		m_col[i]->switchToInput();
	}
}

uint8_t MatrixKeyboard::getKeyPressed()
{
	if (systemClock->getTime() - m_lastKeypressTime < antiBounceDelay)
		return keyNotPressed;
	for (uint8_t i=0; i<m_rowCount; ++i)
	{
		m_row[i]->reset();
		systemClock->wait_us(20);
		for (uint8_t j=0; j<m_colCount; ++j)
		{
			if (m_col[j]->state() == false)
			{
				m_row[i]->set();
				m_lastKeypressTime = systemClock->getTime();
				return parseKey(i, j);
			}
		}
		m_row[i]->set();
	}
	return keyNotPressed;
}

uint8_t MatrixKeyboard::parseKey(uint8_t row, uint8_t col)
{
	if (row < 3 && col < 3)
	{
		return row*3 + (col+1);
	}
	if (col == 3)
	{
		return keyA + row;
	}
	if (row == 3)
	{
		switch (col)
		{
		case 0: return keyAsterisk;
		case 1: return 0;
		case 2: return keySharp;
		default: return keyInvalid;
		}
	}
	return keyInvalid;
}

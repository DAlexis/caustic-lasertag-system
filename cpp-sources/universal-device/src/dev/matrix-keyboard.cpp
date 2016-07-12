/*
 * matrix-keyboard.cpp
 *
 *  Created on: 10 июл. 2016 г.
 *      Author: alexey
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

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

#ifndef UNIVERSAL_DEVICE_INCLUDE_DEV_MATRIX_KEYBOARD_HPP_
#define UNIVERSAL_DEVICE_INCLUDE_DEV_MATRIX_KEYBOARD_HPP_

#include "hal/io-pins.hpp"
#include "core/os-wrappers.hpp"
#include "rcsp/base-types.hpp"

class MatrixKeyboard
{
public:
	constexpr static TimeInterval antiBounceDelay = 100000;

	constexpr static uint8_t keyA = 'A';
	constexpr static uint8_t keyB = 'B';
	constexpr static uint8_t keyC = 'C';
	constexpr static uint8_t keyD = 'D';
	constexpr static uint8_t keySharp    = '#';
	constexpr static uint8_t keyAsterisk = '*';
	constexpr static uint8_t keyNotPressed  = 254;
	constexpr static uint8_t keyInvalid     = 255;


	void init(uint8_t rowCount, uint8_t colCount, IIOPin** row, IIOPin** col);
	uint8_t getKeyPressed();

private:
	static uint8_t parseKey(uint8_t row, uint8_t col);
	uint8_t m_rowCount = 0;
	uint8_t m_colCount = 0;
	IIOPin** m_row = nullptr;
	IIOPin** m_col = nullptr;

	Time m_lastKeypressTime = 0;
};







#endif /* UNIVERSAL_DEVICE_INCLUDE_DEV_MATRIX_KEYBOARD_HPP_ */

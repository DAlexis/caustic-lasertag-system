/*
 * matrix-keyboard.hpp
 *
 *  Created on: 10 июл. 2016 г.
 *      Author: alexey
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

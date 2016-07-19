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

#ifndef INCLUDE_DEV_LCD5110_HPP_
#define INCLUDE_DEV_LCD5110_HPP_

#include "hal/spi.hpp"
#include "hal/io-pins.hpp"
#include "rcsp/RCSP-aggregator.hpp"
#include "rcsp/operation-codes.hpp"

class LCD5110Controller : public ISPIUser
{
public:
	struct LcdIO
	{
		ISPIManager* spi;
		IIOPin* chipEnable;
		IIOPin* dataCommand;
		IIOPin* reset;
	};

	constexpr static uint8_t black = 1;
	constexpr static uint8_t white = 0;

	constexpr static uint8_t fontStandardAscii5x7 = 0;
	constexpr static uint8_t fontLiberationSans15x21Numbers = 1;

	PAR_ST(RESTORABLE, ConfigCodes::AnyDevice::Configuration, lcdContrast);

	LCD5110Controller();

	void configureSPI() override;

	void init(const LcdIO& io);
	void line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
	void circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
	void fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
	void rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
	void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
	void rectLineShadow(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t tx, uint8_t ty, uint8_t color);
	void stringXY(uint8_t x, uint8_t y, char *c);
	void barHorizontal(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t val);
	void barVertical(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t val);

	void setFont(uint8_t font);

	void clearBuffer();
	void updateContrast();

	void write();
private:
	bool m_fontIsTiny = true;
};





#endif /* INCLUDE_DEV_LCD5110_HPP_ */

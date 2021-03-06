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

#include "dev/lcd5110.hpp"
#include "hal/system-clock.hpp"
#include "core/logging.hpp"
#include "utils/memory.hpp"
#include "glcd.h"
#include "fonts/font5x7.h"
#include "fonts/Liberation_Sans15x21_Numbers.h"

#include <stdio.h>


static LCD5110Controller::LcdIO lcdio;

extern "C"
{
	void GLCD_SELECT_impl(void)
	{
		lcdio.chipEnable->reset();
	}

	void GLCD_DESELECT_impl()
	{
		lcdio.chipEnable->set();
	}

	void GLCD_DC_LOW_impl()
	{
		lcdio.dataCommand->reset();
	}

	void GLCD_DC_HIGH_impl()
	{
		lcdio.dataCommand->set();
	}

	void GLCD_RESET_LOW_impl()
	{
		lcdio.reset->reset();
	}

	void GLCD_RESET_HIGH_impl()
	{
		lcdio.reset->set();
	}

	void delay_ms(uint32_t ms)
	{
		//info << "wait";
		systemClock->wait_us(ms*1000);
	}

	void glcd_init(void)
	{
		lcdio.chipEnable->switchToOutput();
		lcdio.dataCommand->switchToOutput();
		lcdio.reset->switchToOutput();
		lcdio.reset->set();
		lcdio.dataCommand->set();
		GLCD_DESELECT();

		lcdio.spi->init(ISPIManager::BaudRatePrescaler32, lcdio.chipEnable, ISPIManager::SPIPhase2edge);

		glcd_PCD8544_init();
		glcd_select_screen(glcd_buffer, &glcd_bbox);
		glcd_clear();
	}

	void glcd_spi_write(uint8_t c)
	{
		GLCD_SELECT_impl();
		lcdio.spi->Transmit(c);
		GLCD_DESELECT_impl();
	}

	void glcd_reset(void)
	{
		GLCD_SELECT_impl();
		GLCD_RESET_LOW_impl();
		delay_ms(GLCD_RESET_TIME);
		GLCD_RESET_HIGH_impl();
		GLCD_DESELECT_impl();
	}
}

LCD5110Controller::LCD5110Controller(RCSPAggregator &aggreagetor) :
		m_aggreagetor(aggreagetor)
{
	lcdContrast = 72; // This worked for one module...
}

void LCD5110Controller::configureSPI()
{
	lcdio.spi->init(ISPIManager::BaudRatePrescaler32, lcdio.chipEnable, ISPIManager::SPIPhase2edge);
}

void LCD5110Controller::init(const LcdIO& io)
{
	lcdio = io;
	glcd_init();

	updateContrast();
	setFont(LCD5110Controller::fontStandardAscii5x7);
}

void LCD5110Controller::updateContrast()
{
	glcd_set_contrast(lcdContrast);
}

void LCD5110Controller::line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
	glcd_draw_line(x1, y1, x2, y2, color);
}

void LCD5110Controller::circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
	glcd_draw_circle(x0, y0, r, color);
}

void LCD5110Controller::fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
	glcd_fill_circle(x0, y0, r, color);
}

void LCD5110Controller::rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
	glcd_draw_rect(x, y, w, h, color);
}

void LCD5110Controller::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
	glcd_fill_rect(x, y, w, h, color);
}

void LCD5110Controller::rectLineShadow(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t tx, uint8_t ty, uint8_t color)
{
	glcd_draw_rect_thick(x, y, w, h, tx, ty, color);
}

void LCD5110Controller::stringXY(uint8_t x, uint8_t y, const char *c)
{
	if (m_fontIsTiny)
		glcd_tiny_draw_string(x, y, c);
	else
		glcd_draw_string_xy(x, y, c);
}

void LCD5110Controller::barHorizontal(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t val)
{
	glcd_bar_graph_horizontal(x, y, width, height, val);
}

void LCD5110Controller::barVertical(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t val)
{
	glcd_bar_graph_vertical(x, y, width, height, val);
}

void LCD5110Controller::setFont(uint8_t font)
{
	switch(font)
	{
	default:
	case fontStandardAscii5x7:
		glcd_tiny_set_font(Font5x7,5,7,32,127);
		m_fontIsTiny = true;
		break;
	case fontLiberationSans15x21Numbers:
		glcd_set_font(Liberation_Sans15x21_Numbers,15,21,46,57);
		m_fontIsTiny = false;
		break;
	}
}

void LCD5110Controller::clearBuffer()
{
	glcd_clear_buffer();
}

void LCD5110Controller::write()
{
	glcd_write();
}

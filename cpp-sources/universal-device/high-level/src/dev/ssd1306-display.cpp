/**
 * original author:  Tilen Majerle<tilen@majerle.eu>
 * modification for STM32f10x: Alexander Lutsai<s.lyra@ya.ru>
 * cpp-wrapper, adaptation for Caustic project: Alexey Bulatov <AlexeyABulatov@yandex.ru>
   ----------------------------------------------------------------------
    Copyright (C) Alexander Lutsai, 2016
    Copyright (C) Tilen Majerle, 2015
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
 */

#include "dev/ssd1306-display.hpp"
#include "core/logging.hpp"
#include "hal/system-clock.hpp"
#include "string.h"

// Absolute value
#define ABS(x)   ((x) > 0 ? (x) : -(x))

bool SSD1306Display::init(II2CManager* i2c)
{
    m_i2c = i2c;

    debug << "SSD1306 initialization";
    // Init I2C
    m_i2c->init();

    // Check if LCD connected to I2C
    if (!m_i2c->isDeviceReady(m_addr, 1, 1000))
    {
        info << "SSD1306 I2C device not found";
        return false;
    }

    // A little delay
    systemClock->wait_us(150);

    // Init LCD
    writeCommand(0xAE); //display off
    writeCommand(0x20); //Set Memory Addressing Mode
    writeCommand(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    writeCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
    writeCommand(0xC8); //Set COM Output Scan Direction
    writeCommand(0x00); //---set low column address
    writeCommand(0x10); //---set high column address
    writeCommand(0x40); //--set start line address
    writeCommand(0x81); //--set contrast control register
    writeCommand(0xFF);
    writeCommand(0xA1); //--set segment re-map 0 to 127
    writeCommand(0xA6); //--set normal display
    writeCommand(0xA8); //--set multiplex ratio(1 to 64)
    writeCommand(0x3F); //
    writeCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    writeCommand(0xD3); //-set display offset
    writeCommand(0x00); //-not offset
    writeCommand(0xD5); //--set display clock divide ratio/oscillator frequency
    writeCommand(0xF0); //--set divide ratio
    writeCommand(0xD9); //--set pre-charge period
    writeCommand(0x22); //
    writeCommand(0xDA); //--set com pins hardware configuration
    writeCommand(0x12);
    writeCommand(0xDB); //--set vcomh
    writeCommand(0x20); //0x20,0.77xVcc
    writeCommand(0x8D); //--set DC-DC enable
    writeCommand(0x14); //
    writeCommand(0xAF); //--turn on SSD1306 panel

    // Clear screen
    Fill(Color::black);

    // Update screen
    updateScreen();

    // Set default values
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    // Initialized OK
    SSD1306.Initialized = 1;

    debug << "SSD1306 init OK";

    // Return OK
    return true;
}

void SSD1306Display::updateScreen(void)
{
    SSD1306_Buffer_all[0] = 0x40;
    m_i2c->transmit(m_addr, SSD1306_Buffer_all, m_width * m_height/ 8 + 1);
}

void SSD1306Display::ToggleInvert(void)
{
    uint16_t i;

    /* Toggle invert */
    SSD1306.Inverted = !SSD1306.Inverted;

    /* Do memory toggle */
    for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = ~SSD1306_Buffer[i];
    }
}

void SSD1306Display::Fill(uint8_t color)
{
    memset(SSD1306_Buffer, (color == Color::black) ? 0x00 : 0xFF, m_width * m_height / 8);
}

void SSD1306Display::DrawPixel(uint16_t x, uint16_t y, uint8_t color)
{
    if (
       x >= m_width ||
       y >= m_height
    ) {
       /* Error */
       return;
    }

    /* Check if pixels are inverted */
    if (SSD1306.Inverted) {
       color = (uint8_t)!color;
    }

    /* Set color */
    if (color == Color::white) {
       SSD1306_Buffer[x + (y / 8) * m_width] |= 1 << (y % 8);
    } else {
       SSD1306_Buffer[x + (y / 8) * m_width] &= ~(1 << (y % 8));
    }
}

void SSD1306Display::GotoXY(uint16_t x, uint16_t y)
{
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}


char SSD1306Display::Putc(char ch, FontDef_t* Font, uint8_t color)
{
    uint32_t i, b, j;

    b = 0;
    // Go through font
    for (i = 0; i < Font->FontHeight; i++) {
        for (j = 0; j < Font->FontWidth; j++) {
            if ((Font->data[ch*Font->CharBytes + b/8] >> b%8) & 1) {
                DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (uint8_t) color);
            } else {
                DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (uint8_t)!color);
            }
            b++;
        }
    }

    // Increase pointer
    SSD1306.CurrentX += Font->FontWidth;

    // Return character written
    return ch;
}

char SSD1306Display::Puts(char* str, FontDef_t* Font, uint8_t color)
{
    // Write characters
    while (*str) {
        // Write character by character
        if (Putc(*str, Font, color) != *str) {
            // Return error
            return *str;
        }

        // Increase string pointer
        str++;
    }

    // Everything OK, zero should be returned
    return *str;
}

void SSD1306Display::DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t c)
{
    int16_t dx, dy, sx, sy, err, e2, i, tmp;

    /* Check for overflow */
    if (x0 >= m_width) {
        x0 = m_width - 1;
    }
    if (x1 >= m_width) {
        x1 = m_width - 1;
    }
    if (y0 >= m_height) {
        y0 = m_height - 1;
    }
    if (y1 >= m_height) {
        y1 = m_height - 1;
    }

    dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
    dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
    sx = (x0 < x1) ? 1 : -1;
    sy = (y0 < y1) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;

    if (dx == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Vertical line */
        for (i = y0; i <= y1; i++) {
            DrawPixel(x0, i, c);
        }

        /* Return from function */
        return;
    }

    if (dy == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Horizontal line */
        for (i = x0; i <= x1; i++) {
            DrawPixel(i, y0, c);
        }

        /* Return from function */
        return;
    }

    while (1) {
        DrawPixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

void SSD1306Display::DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t c)
{
    /* Check input parameters */
    if (
        x >= m_width ||
        y >= m_height
    ) {
        /* Return error */
        return;
    }

    /* Check width and height */
    if ((x + w) >= m_width) {
        w = m_width - x;
    }
    if ((y + h) >= m_height) {
        h = m_height - y;
    }

    /* Draw 4 lines */
    DrawLine(x, y, x + w, y, c);         /* Top line */
    DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
    DrawLine(x, y, x, y + h, c);         /* Left line */
    DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void SSD1306Display::DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t c)
{
    uint8_t i;

    /* Check input parameters */
    if (
        x >= m_width ||
        y >= m_height
    ) {
        /* Return error */
        return;
    }

    /* Check width and height */
    if ((x + w) >= m_width) {
        w = m_width - x;
    }
    if ((y + h) >= m_height) {
        h = m_height - y;
    }

    /* Draw lines */
    for (i = 0; i <= h; i++) {
        /* Draw lines */
        DrawLine(x, y + i, x + w, y + i, c);
    }
}

void SSD1306Display::DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color)
{
    /* Draw lines */
    DrawLine(x1, y1, x2, y2, color);
    DrawLine(x2, y2, x3, y3, color);
    DrawLine(x3, y3, x1, y1, color);
}

void SSD1306Display::DrawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t c)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    DrawPixel(x0, y0 + r, c);
    DrawPixel(x0, y0 - r, c);
    DrawPixel(x0 + r, y0, c);
    DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        DrawPixel(x0 + x, y0 + y, c);
        DrawPixel(x0 - x, y0 + y, c);
        DrawPixel(x0 + x, y0 - y, c);
        DrawPixel(x0 - x, y0 - y, c);

        DrawPixel(x0 + y, y0 + x, c);
        DrawPixel(x0 - y, y0 + x, c);
        DrawPixel(x0 + y, y0 - x, c);
        DrawPixel(x0 - y, y0 - x, c);
    }
}

void SSD1306Display::DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint8_t c)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    DrawPixel(x0, y0 + r, c);
    DrawPixel(x0, y0 - r, c);
    DrawPixel(x0 + r, y0, c);
    DrawPixel(x0 - r, y0, c);
    DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}

void SSD1306Display::image(uint8_t *img, uint8_t frame, uint8_t x, uint8_t y)
{
    uint32_t i, b, j;

    b = 0;
    if(frame >= img[2])
        return;
    uint32_t start = (frame * (img[3] + (img[4] << 8)));

    /* Go through font */
    for (i = 0; i < img[1]; i++) {
        for (j = 0; j < img[0]; j++) {

            DrawPixel(x + j, (y + i), (uint8_t) (img[b/8 + 5 + start] >> (b%8)) & 1);
            b++;
        }
    }
}


void SSD1306Display::writeReg(uint8_t reg, uint8_t data)
{
    uint8_t dt[2];
    dt[0] = reg;
    dt[1] = data;
    m_i2c->transmit(m_addr, dt, 2);
}

void SSD1306Display::writeCommand(uint8_t command)
{
    writeReg(0x00, command);
}

void SSD1306Display::writeData(uint8_t command)
{
    writeReg(0x40, command);
}

const uint8_t Font7x10[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x87, 0xe3, 0xe3, 0x70, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x15, 0x45, 0x45, 0x41, 0x51, 0x54, 0x00, 0x00, 0x00,  //
    0x00, 0x89, 0xc4, 0x23, 0x91, 0xf0, 0x20, 0x10, 0x08, 0x00,  //
    0x00, 0x87, 0xc0, 0x20, 0xd0, 0x21, 0x30, 0x08, 0x04, 0x00,  //
    0x00, 0x8e, 0x40, 0xc0, 0xe1, 0x90, 0x38, 0x24, 0x12, 0x00,  //
    0x00, 0x81, 0x40, 0xe0, 0xe1, 0x11, 0x38, 0x04, 0x02, 0x00,  //
    0x00, 0x0e, 0x85, 0xc2, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x02, 0xe1, 0x43, 0x20, 0x7c, 0x00, 0x00, 0x00,  //
    0x00, 0x89, 0xc5, 0xa2, 0x91, 0x10, 0x08, 0x04, 0x1e, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x08, 0x04, 0x02, 0xf1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0xf0, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00,  //
    0x00, 0x00, 0x00, 0x80, 0x43, 0x20, 0x10, 0x08, 0x04, 0x00,  //
    0x08, 0x04, 0x02, 0x81, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x08, 0x04, 0x02, 0xf1, 0x43, 0x20, 0x10, 0x08, 0x04, 0x00,  //
    0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00,  //
    0x08, 0x04, 0x02, 0x81, 0x43, 0x20, 0x10, 0x08, 0x04, 0x00,  //
    0x08, 0x04, 0x02, 0xf1, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00,  //
    0x08, 0x04, 0x02, 0xf1, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0xf0, 0x43, 0x20, 0x10, 0x08, 0x04, 0x00,  //
    0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00,  //
    0x00, 0x10, 0x84, 0x20, 0x20, 0x40, 0x40, 0x3e, 0x00, 0x00,  //
    0x00, 0x01, 0x01, 0x02, 0x82, 0x10, 0x04, 0x3e, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x47, 0xa1, 0x50, 0x28, 0x00, 0x00, 0x00,  //
    0x00, 0x10, 0xc4, 0x87, 0xf0, 0x11, 0x04, 0x00, 0x00, 0x00,  //
    0x00, 0x0c, 0x89, 0xe0, 0x21, 0x70, 0x6c, 0x04, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x02, 0x81, 0x40, 0x00, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x0a, 0x85, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x0a, 0xc5, 0x47, 0xf1, 0x51, 0x28, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x47, 0xc1, 0x41, 0x71, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x92, 0x8a, 0x82, 0xa0, 0xa8, 0x24, 0x00, 0x00, 0x00,  //
    0x00, 0x82, 0x42, 0x41, 0x50, 0x49, 0x58, 0x00, 0x00, 0x00,  //
    0x00, 0x0c, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x08, 0x82, 0x40, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00,  //
    0x00, 0x02, 0x02, 0x02, 0x81, 0x20, 0x08, 0x00, 0x00, 0x00,  //
    0x00, 0x80, 0x88, 0xe2, 0xa3, 0x88, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x02, 0xe1, 0x43, 0x20, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x04, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x38, 0x08, 0x00, 0x00,  //
    0x00, 0x10, 0x08, 0x82, 0x20, 0x08, 0x04, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x45, 0x24, 0x12, 0x51, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x43, 0x81, 0x40, 0x20, 0x7c, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x08, 0x84, 0x21, 0x08, 0x7c, 0x00, 0x00, 0x00,  //
    0x00, 0x1f, 0x08, 0x82, 0x01, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x08, 0x86, 0x22, 0xf1, 0x41, 0x20, 0x00, 0x00, 0x00,  //
    0x00, 0x9f, 0x40, 0x63, 0x02, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x0c, 0x41, 0xa0, 0x31, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x1f, 0x08, 0x02, 0x41, 0x10, 0x08, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x48, 0xc4, 0x11, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x48, 0xc6, 0x02, 0x41, 0x18, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x82, 0x83, 0x00, 0x20, 0x38, 0x08, 0x00, 0x00,  //
    0x00, 0x00, 0x82, 0x83, 0x00, 0x60, 0x10, 0x04, 0x00, 0x00,  //
    0x00, 0x10, 0x04, 0x41, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x07, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x01, 0x01, 0x01, 0x41, 0x10, 0x04, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x08, 0x82, 0x40, 0x00, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x48, 0xa6, 0xd2, 0x08, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x45, 0x24, 0xf2, 0x89, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x0f, 0x89, 0xc4, 0x21, 0x91, 0x3c, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x48, 0x20, 0x10, 0x88, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x0f, 0x89, 0x44, 0x22, 0x91, 0x3c, 0x00, 0x00, 0x00,  //
    0x00, 0x9f, 0x40, 0xe0, 0x11, 0x08, 0x7c, 0x00, 0x00, 0x00,  //
    0x00, 0x9f, 0x40, 0xe0, 0x11, 0x08, 0x04, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x48, 0x20, 0x90, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x91, 0x48, 0xe4, 0x13, 0x89, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x0e, 0x02, 0x81, 0x40, 0x20, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x1c, 0x08, 0x04, 0x02, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x91, 0x44, 0x61, 0x50, 0x48, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x81, 0x40, 0x20, 0x10, 0x08, 0x7c, 0x00, 0x00, 0x00,  //
    0x00, 0x91, 0xc8, 0xa6, 0x12, 0x89, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x91, 0xc8, 0xa4, 0x92, 0x89, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x48, 0x24, 0x12, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x8f, 0x48, 0xe4, 0x11, 0x08, 0x04, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x48, 0x24, 0x12, 0xa9, 0x38, 0x20, 0x00, 0x00,  //
    0x00, 0x8f, 0x48, 0xe4, 0x51, 0x48, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x48, 0xc0, 0x01, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x1f, 0x02, 0x81, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x91, 0x48, 0x24, 0x12, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x91, 0x48, 0x44, 0xa1, 0x50, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x91, 0x48, 0xa4, 0x52, 0xd9, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x91, 0x88, 0x82, 0xa0, 0x88, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x91, 0x88, 0x82, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x1f, 0x08, 0x82, 0x20, 0x08, 0x7c, 0x00, 0x00, 0x00,  //
    0x00, 0x0e, 0x81, 0x40, 0x20, 0x10, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x81, 0x80, 0x80, 0x80, 0x80, 0x40, 0x00, 0x00, 0x00,  //
    0x00, 0x0e, 0x04, 0x02, 0x81, 0x40, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x45, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00,  //
    0x00, 0x06, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x80, 0x03, 0xe2, 0x89, 0x78, 0x00, 0x00, 0x00,  //
    0x00, 0x81, 0x40, 0x63, 0x12, 0x99, 0x34, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x80, 0x23, 0x12, 0x88, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x10, 0x88, 0x25, 0x13, 0xc9, 0x58, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x80, 0x23, 0xf2, 0x09, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x0c, 0x89, 0xe0, 0x21, 0x10, 0x08, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x80, 0x25, 0x61, 0x08, 0x38, 0x22, 0x0e, 0x00,  //
    0x00, 0x81, 0x40, 0x63, 0x12, 0x89, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x80, 0x81, 0x40, 0x20, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x08, 0x00, 0x03, 0x81, 0x40, 0x24, 0x12, 0x06, 0x00,  //
    0x00, 0x81, 0x40, 0x24, 0x71, 0x48, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x06, 0x02, 0x81, 0x40, 0x20, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0xa2, 0x52, 0xa9, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x40, 0x63, 0x12, 0x89, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x80, 0x23, 0x12, 0x89, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x40, 0x63, 0x32, 0x69, 0x04, 0x02, 0x01, 0x00,  //
    0x00, 0x00, 0x80, 0x25, 0x93, 0xb1, 0x40, 0x20, 0x10, 0x00,  //
    0x00, 0x00, 0x40, 0x63, 0x12, 0x08, 0x04, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x80, 0x23, 0xe0, 0x80, 0x3c, 0x00, 0x00, 0x00,  //
    0x00, 0x02, 0xc1, 0x43, 0x20, 0x90, 0x30, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x40, 0x24, 0x12, 0xc9, 0x58, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x40, 0x24, 0xa2, 0x50, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x40, 0x24, 0x52, 0xa9, 0x28, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x40, 0x44, 0x41, 0x50, 0x44, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x40, 0x24, 0x92, 0xb1, 0x40, 0x22, 0x0e, 0x00,  //
    0x00, 0x00, 0xc0, 0x07, 0x41, 0x10, 0x7c, 0x00, 0x00, 0x00,  //
    0x00, 0x18, 0x02, 0xc2, 0x80, 0x20, 0x60, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00,  //
    0x00, 0x03, 0x82, 0x80, 0x21, 0x20, 0x0c, 0x00, 0x00, 0x00,  //
    0x00, 0x92, 0x4a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x14, 0x80, 0x4f, 0xe0, 0x11, 0x08, 0x7c, 0x00, 0x00, 0x00,  //
    0x1c, 0x51, 0xb2, 0x5a, 0xcc, 0x8a, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x04, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x0a, 0x80, 0x23, 0xf2, 0x09, 0x38, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
    0x00, 0x82, 0x22, 0x12, 0xf9, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x80, 0x47, 0x20, 0xf0, 0x88, 0x44, 0x1e, 0x00, 0x00, 0x00,  //
    0x80, 0x47, 0x24, 0xf2, 0x88, 0x44, 0x1e, 0x00, 0x00, 0x00,  //
    0x80, 0x4f, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00, 0x00,  //
    0x00, 0x0c, 0x85, 0x22, 0x91, 0x48, 0x3e, 0x11, 0x00, 0x00,  //
    0x80, 0x4f, 0x20, 0xf0, 0x08, 0x04, 0x3e, 0x00, 0x00, 0x00,  //
    0x80, 0x4a, 0xa5, 0xe2, 0xa8, 0x54, 0x2a, 0x00, 0x00, 0x00,  //
    0x00, 0x47, 0x04, 0xc2, 0x80, 0x44, 0x1c, 0x00, 0x00, 0x00,  //
    0x80, 0x48, 0x24, 0x53, 0x99, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x0a, 0x42, 0x24, 0x53, 0x99, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x80, 0x48, 0xa2, 0x30, 0x28, 0x24, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x8e, 0x44, 0x22, 0x91, 0x48, 0x22, 0x00, 0x00, 0x00,  //
    0x80, 0x48, 0x64, 0x53, 0x89, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x80, 0x48, 0x24, 0xf2, 0x89, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x47, 0x24, 0x12, 0x89, 0x44, 0x1c, 0x00, 0x00, 0x00,  //
    0x80, 0x4f, 0x24, 0x12, 0x89, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x80, 0x47, 0x24, 0xf2, 0x08, 0x04, 0x02, 0x00, 0x00, 0x00,  //
    0x00, 0x47, 0x24, 0x10, 0x08, 0x44, 0x1c, 0x00, 0x00, 0x00,  //
    0x80, 0x0f, 0x81, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00,  //
    0x80, 0x48, 0x24, 0xa2, 0x20, 0x08, 0x02, 0x00, 0x00, 0x00,  //
    0x00, 0x82, 0xa3, 0x52, 0xa9, 0x38, 0x08, 0x00, 0x00, 0x00,  //
    0x80, 0x48, 0x44, 0x41, 0x50, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x80, 0x44, 0x22, 0x91, 0x48, 0x24, 0x3e, 0x10, 0x08, 0x00,  //
    0x80, 0x48, 0x24, 0xe2, 0x81, 0x40, 0x20, 0x00, 0x00, 0x00,  //
    0x80, 0x4a, 0xa5, 0x52, 0xa9, 0x54, 0x3e, 0x00, 0x00, 0x00,  //
    0x80, 0x4a, 0xa5, 0x52, 0xa9, 0x54, 0x3e, 0x10, 0x08, 0x00,  //
    0x80, 0x81, 0x40, 0xe0, 0x90, 0x48, 0x1c, 0x00, 0x00, 0x00,  //
    0x80, 0x48, 0x24, 0x72, 0xc9, 0x64, 0x2e, 0x00, 0x00, 0x00,  //
    0x00, 0x81, 0x40, 0xe0, 0x90, 0x48, 0x1c, 0x00, 0x00, 0x00,  //
    0x00, 0x47, 0x04, 0xc2, 0x81, 0x44, 0x1c, 0x00, 0x00, 0x00,  //
    0x80, 0x44, 0xa5, 0x72, 0xa9, 0x54, 0x12, 0x00, 0x00, 0x00,  //
    0x00, 0x4f, 0x24, 0xe2, 0xa1, 0x48, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x01, 0xf1, 0x44, 0x3c, 0x00, 0x00, 0x00,  //
    0x00, 0x4f, 0xc0, 0x11, 0x89, 0x44, 0x1c, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xe0, 0x11, 0x79, 0x44, 0x1e, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x23, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x80, 0xa1, 0x50, 0x28, 0x3e, 0x11, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x11, 0xf9, 0x04, 0x1c, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xa0, 0x52, 0x71, 0x54, 0x2a, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x11, 0x61, 0x44, 0x1c, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x20, 0x92, 0xa9, 0x4c, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x05, 0x21, 0x92, 0xa9, 0x4c, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x20, 0x92, 0x38, 0x24, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x80, 0x23, 0x91, 0x48, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x20, 0xb2, 0xa9, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x20, 0x12, 0xf9, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x11, 0x89, 0x44, 0x1c, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xe0, 0x13, 0x89, 0x44, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xa0, 0x31, 0x89, 0x4c, 0x1a, 0x81, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x11, 0x09, 0x44, 0x1c, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xe0, 0x43, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x20, 0x12, 0x89, 0x78, 0x20, 0x11, 0x07, 0x00,  //
    0x00, 0x02, 0xc1, 0x51, 0xa9, 0x54, 0x1c, 0x04, 0x02, 0x00,  //
    0x00, 0x00, 0x20, 0xa2, 0x20, 0x28, 0x22, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x20, 0x91, 0x48, 0x24, 0x3e, 0x10, 0x08, 0x00,  //
    0x00, 0x00, 0x20, 0x12, 0xf1, 0x40, 0x20, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xa0, 0x52, 0xa9, 0x54, 0x3e, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xa0, 0x52, 0xa9, 0x54, 0x3e, 0x10, 0x08, 0x00,  //
    0x00, 0x00, 0x60, 0x20, 0x70, 0x48, 0x1c, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x20, 0x12, 0xb9, 0x64, 0x2e, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x40, 0x20, 0x70, 0x48, 0x1c, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x11, 0xe1, 0x44, 0x1c, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0x20, 0x51, 0xb9, 0x54, 0x12, 0x00, 0x00, 0x00,  //
    0x00, 0x00, 0xc0, 0x13, 0xf1, 0x44, 0x22, 0x00, 0x00, 0x00,  //
};



const uint8_t Font4x6[] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x22, 0x02, 0x02, 0x00,
    0x55, 0x00, 0x00, 0x00,
    0x75, 0x75, 0x05, 0x00,
    0x36, 0x36, 0x02, 0x00,
    0x41, 0x12, 0x04, 0x00,
    0x33, 0x57, 0x06, 0x00,
    0x22, 0x00, 0x00, 0x00,
    0x24, 0x22, 0x04, 0x00,
    0x21, 0x22, 0x01, 0x00,
    0x25, 0x05, 0x00, 0x00,
    0x20, 0x27, 0x00, 0x00,
    0x00, 0x20, 0x01, 0x00,
    0x00, 0x07, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x00,
    0x44, 0x12, 0x01, 0x00,
    0x56, 0x55, 0x03, 0x00,
    0x32, 0x22, 0x02, 0x00,
    0x43, 0x12, 0x07, 0x00,
    0x43, 0x42, 0x03, 0x00,
    0x55, 0x47, 0x04, 0x00,
    0x17, 0x43, 0x03, 0x00,
    0x16, 0x57, 0x07, 0x00,
    0x47, 0x12, 0x01, 0x00,
    0x57, 0x57, 0x07, 0x00,
    0x57, 0x47, 0x03, 0x00,
    0x20, 0x20, 0x00, 0x00,
    0x20, 0x20, 0x01, 0x00,
    0x24, 0x21, 0x04, 0x00,
    0x70, 0x70, 0x00, 0x00,
    0x21, 0x24, 0x01, 0x00,
    0x47, 0x02, 0x02, 0x00,
    0x52, 0x17, 0x06, 0x00,
    0x52, 0x57, 0x05, 0x00,
    0x53, 0x53, 0x03, 0x00,
    0x16, 0x11, 0x06, 0x00,
    0x53, 0x55, 0x03, 0x00,
    0x17, 0x17, 0x07, 0x00,
    0x17, 0x17, 0x01, 0x00,
    0x16, 0x57, 0x06, 0x00,
    0x55, 0x57, 0x05, 0x00,
    0x27, 0x22, 0x07, 0x00,
    0x44, 0x54, 0x02, 0x00,
    0x55, 0x53, 0x05, 0x00,
    0x11, 0x11, 0x07, 0x00,
    0x75, 0x57, 0x05, 0x00,
    0x75, 0x77, 0x05, 0x00,
    0x52, 0x55, 0x02, 0x00,
    0x53, 0x13, 0x01, 0x00,
    0x52, 0x75, 0x06, 0x00,
    0x53, 0x37, 0x05, 0x00,
    0x16, 0x42, 0x03, 0x00,
    0x27, 0x22, 0x02, 0x00,
    0x55, 0x55, 0x06, 0x00,
    0x55, 0x25, 0x02, 0x00,
    0x55, 0x77, 0x05, 0x00,
    0x55, 0x52, 0x05, 0x00,
    0x55, 0x22, 0x02, 0x00,
    0x47, 0x12, 0x07, 0x00,
    0x17, 0x11, 0x07, 0x00,
    0x10, 0x42, 0x00, 0x00,
    0x47, 0x44, 0x07, 0x00,
    0x52, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x07, 0x00,
    0x21, 0x00, 0x00, 0x00,
    0x30, 0x56, 0x07, 0x00,
    0x31, 0x55, 0x03, 0x00,
    0x60, 0x11, 0x06, 0x00,
    0x64, 0x55, 0x06, 0x00,
    0x60, 0x35, 0x06, 0x00,
    0x24, 0x27, 0x02, 0x00,
    0x60, 0x75, 0x24, 0x00,
    0x31, 0x55, 0x05, 0x00,
    0x02, 0x22, 0x02, 0x00,
    0x04, 0x44, 0x25, 0x00,
    0x51, 0x33, 0x05, 0x00,
    0x23, 0x22, 0x07, 0x00,
    0x70, 0x77, 0x05, 0x00,
    0x30, 0x55, 0x05, 0x00,
    0x20, 0x55, 0x02, 0x00,
    0x30, 0x55, 0x13, 0x00,
    0x60, 0x55, 0x46, 0x00,
    0x60, 0x11, 0x01, 0x00,
    0x60, 0x63, 0x03, 0x00,
    0x72, 0x22, 0x06, 0x00,
    0x50, 0x55, 0x06, 0x00,
    0x50, 0x75, 0x02, 0x00,
    0x50, 0x77, 0x07, 0x00,
    0x50, 0x22, 0x05, 0x00,
    0x50, 0x65, 0x24, 0x00,
    0x70, 0x36, 0x07, 0x00,
    0x26, 0x21, 0x06, 0x00,
    0x22, 0x20, 0x02, 0x00,
    0x23, 0x24, 0x03, 0x00,
    0x36, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

FontDef_t Font_4x6 = {
    4,
    6,
    4,
    Font4x6
};
FontDef_t Font_7x10 = {
    7,
    10,
    10,
    Font7x10
};

////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////
/*
void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color) {
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
    curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x = x1;
    y = y1;

    if (x2 >= x1) {
        xinc1 = 1;
        xinc2 = 1;
    } else {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {
        yinc1 = 1;
        yinc2 = 1;
    } else {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay){
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    } else {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        SSD1306_DrawLine(x, y, x3, y3, color);

        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}*/

/*
void SSD1306_ON(void) {
    writeCommand(0x8D);
    writeCommand(0x14);
    writeCommand(0xAF);
}
void SSD1306_OFF(void) {
    writeCommand(0x8D);
    writeCommand(0x10);
    writeCommand(0xAE);
}*/
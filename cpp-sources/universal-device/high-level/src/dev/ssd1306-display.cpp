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

// Black color, no pixel
#define SSD1306_COLOR_BLACK 0x00

// Pixel is set. Color depends on LCD
#define SSD1306_COLOR_WHITE 0x01

bool SSD1306Display::init(II2CManager* i2c)
{
    m_i2c = i2c;


    debug << "SSD1306 initialization";
    // Init I2C
    m_i2c->init();
    /*debug << "test transmit";
    m_i2c->transmit(m_addr, (uint8_t*)"\0", 1);
    debug << "test transmit end";*/
    // Check if LCD connected to I2C
    if (!m_i2c->isDeviceReady(m_addr, 1, 1000))
    {
        info << "SSD1306 I2C device not found";
        return false;
    }

    systemClock->wait_us(15);
    /* A little delay */
    uint32_t p = 2500;
    while(p>0)
        p--;

    /* Init LCD */
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

    /* Clear screen */
    Fill(SSD1306_COLOR_BLACK);

    /* Update screen */
    updateScreen();

    /* Set default values */
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    /* Initialized OK */
    SSD1306.Initialized = 1;

    debug << "SSD1306 init OK";

    /* Return OK */
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
    memset(SSD1306_Buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, m_width * m_height / 8);
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
    if (color == SSD1306_COLOR_WHITE) {
       SSD1306_Buffer[x + (y / 8) * m_width] |= 1 << (y % 8);
    } else {
       SSD1306_Buffer[x + (y / 8) * m_height] &= ~(1 << (y % 8));
    }
}

void SSD1306Display::GotoXY(uint16_t x, uint16_t y)
{
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

/*
char SSD1306Display::Putc(char ch, FontDef_t* Font, uint8_t color)
{
    uint32_t i, b, j;

    b = 0;
    // Go through font
    for (i = 0; i < Font->FontHeight; i++) {
        for (j = 0; j < Font->FontWidth; j++) {
            if ((Font->data[ch*Font->CharBytes + b/8] >> b%8) & 1) {
                SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (uint8_t) color);
            } else {
                SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (uint8_t)!color);
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
        if (SSD1306_Putc(*str, Font, color) != *str) {
            // Return error
            return *str;
        }

        // Increase string pointer
        str++;
    }

    // Everything OK, zero should be returned
    return *str;
}
*/
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

/*
 * ssd1306-display.hpp
 *
 *  Created on: 23 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_DEV_SSD1306_DISPLAY_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_DEV_SSD1306_DISPLAY_HPP_

#include "hal/i2c.hpp"

class SSD1306Display
{
public:
    bool init(II2CManager* i2c);
    void updateScreen(void);
    void ToggleInvert(void);
    void Fill(uint8_t Color);
    void DrawPixel(uint16_t x, uint16_t y, uint8_t color);
    void GotoXY(uint16_t x, uint16_t y);
    /*
    char Putc(char ch, FontDef_t* Font, uint8_t color);
    char Puts(char* str, FontDef_t* Font, uint8_t color);*/
    void DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t c);
    void DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t c);
    void DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t c);
    void DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color);
    void DrawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t c);
    void DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint8_t c);
    void image(uint8_t *img, uint8_t frame, uint8_t x, uint8_t y);

private:
    /* Private SSD1306 structure */
    struct SSD1306_t {
        uint16_t CurrentX;
        uint16_t CurrentY;
        uint8_t Inverted;
        uint8_t Initialized;
    };

    void writeReg(uint8_t reg, uint8_t data);
    void writeCommand(uint8_t command);
    void writeData(uint8_t command);

    II2CManager* m_i2c = nullptr;
    uint16_t m_addr = 0x78;
    constexpr static uint16_t m_width = 128;
    constexpr static uint16_t m_height = 64;

    SSD1306_t SSD1306;
    uint8_t SSD1306_Buffer_all[m_width * m_height / 8 + 1];
    uint8_t *SSD1306_Buffer = SSD1306_Buffer_all + 1;
};







#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_DEV_SSD1306_DISPLAY_HPP_ */

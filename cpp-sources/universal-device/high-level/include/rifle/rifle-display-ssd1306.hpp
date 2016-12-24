/*
 * rifle-display-ssd1306.hpp
 *
 *  Created on: 24 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_DISPLAY_SSD1306_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_DISPLAY_SSD1306_HPP_

#include "rifle/rifle-display.hpp"
#include "dev/ssd1306-display.hpp"

class RifleSSD1306Display : public RifleDisplayBase
{
public:
    bool init() override;
    void update() override;

private:
    SSD1306Display m_display;
};







#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_DISPLAY_SSD1306_HPP_ */

/*
 * rifle-display-lcd5110.hpp
 *
 *  Created on: 24 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_DISPLAY_LCD5110_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_DISPLAY_LCD5110_HPP_

#include "rifle/rifle-display.hpp"
#include "dev/lcd5110.hpp"

class RifleLCD5110Display : public RifleDisplayBase
{
public:
    RifleLCD5110Display(RCSPAggregator &aggreagetor);
    bool init() override;
    void update() override;

private:
    LCD5110Controller m_lcd;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_RIFLE_RIFLE_DISPLAY_LCD5110_HPP_ */

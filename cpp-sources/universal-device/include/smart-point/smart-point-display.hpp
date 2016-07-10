/*
 * smart-point-display.hpp
 *
 *  Created on: 10 июл. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_DISPLAY_HPP_
#define UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_DISPLAY_HPP_

#include "dev/lcd5110.hpp"
#include "smart-point/smart-point-state.hpp"

class SmartPointLCD5110Display
{
public:
	SmartPointLCD5110Display();
	void update();
private:
	LCD5110Controller m_lcd;
};

#endif /* UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_DISPLAY_HPP_ */

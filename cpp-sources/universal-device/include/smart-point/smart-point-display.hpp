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

#ifndef UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_DISPLAY_HPP_
#define UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_DISPLAY_HPP_

#include "smart-point/smart-point-config-and-state.hpp"
#include "dev/lcd5110.hpp"

class SmartPointLCD5110Display
{
public:
	SmartPointLCD5110Display();
	void update();
	void init();

private:

	LCD5110Controller m_lcd;
};

#endif /* UNIVERSAL_DEVICE_INCLUDE_SMART_POINT_SMART_POINT_DISPLAY_HPP_ */

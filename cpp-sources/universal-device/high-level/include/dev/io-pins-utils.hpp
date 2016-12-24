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

#ifndef DEV_IO_PINS_UTILS_HPP_
#define DEV_IO_PINS_UTILS_HPP_

#include "hal/io-pins.hpp"
#include "core/os-wrappers.hpp"

void delayedSwitchPin(TasksPool &pool, IIOPin* pin, bool state, uint32_t delay);
void blinkPin(TasksPool &pool, IIOPin* pin, uint32_t delay);

#endif /* DEV_IO_PINS_UTILS_HPP_ */
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


#ifndef INC_HW_HARDWARE_INITIALIZER_HW_HPP_
#define INC_HW_HARDWARE_INITIALIZER_HW_HPP_

#include "hal/hardware-initializer.hpp"

class HardwareInitializer : public IHardwareInitializer
{
public:
	void init();

private:
	void SystemClock_Config();
	void MX_GPIO_Init(void);
	/* SDIO init function */
	void MX_SDIO_SD_Init(void);
	void MX_DMA_Init(void);
};



#endif /* INC_HW_HARDWARE_INITIALIZER_HW_HPP_ */

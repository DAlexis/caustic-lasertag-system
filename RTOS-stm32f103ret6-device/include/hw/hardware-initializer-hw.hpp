/*
 * hardware-initializer-hw.hpp
 *
 *  Created on: 27 дек. 2015 г.
 *      Author: alexey
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

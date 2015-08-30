/*
 * io-pins-utils.hpp
 *
 *  Created on: 24 авг. 2015 г.
 *      Author: alexey
 */

#ifndef DEV_IO_PINS_UTILS_HPP_
#define DEV_IO_PINS_UTILS_HPP_

#include "hal/io-pins.hpp"
#include "core/os-wrappers.hpp"

void delayedSwitchPin(TasksPool &pool, IIOPin* pin, bool state, uint32_t delay);


#endif /* DEV_IO_PINS_UTILS_HPP_ */

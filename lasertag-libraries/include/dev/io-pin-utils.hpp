/*
 * io-pin-utils.hpp
 *
 *  Created on: 06 июня 2015 г.
 *      Author: alexey
 */

#ifndef INCLUDE_DEV_IO_PIN_UTILS_HPP_
#define INCLUDE_DEV_IO_PIN_UTILS_HPP_

#include "hal/io-pins.hpp"
#include "core/scheduler.hpp"

TaskId delayedSwitchPin(IIOPin* pin, bool state, uint32_t delay);

#endif /* INCLUDE_DEV_IO_PIN_UTILS_HPP_ */

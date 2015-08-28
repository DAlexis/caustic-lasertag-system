/*
 * io-pins.hpp
 *
 *  Created on: 20 июня 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_IO_PINS_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_IO_PINS_HPP_

#include <stdint.h>
#include <functional>

using IOPinCallback = std::function<void(bool)>;

class IIOPin
{
public:
	virtual ~IIOPin() {}
	virtual bool state() = 0;
	/// Switch to state "1"
	virtual void set() = 0;
	/// Switch to state "0"
	virtual void reset() = 0;

	virtual void switchToOutput() = 0;
	virtual void switchToInput() = 0;
	virtual void setExtiCallback(const IOPinCallback& callback, bool direclyFromISR = false) = 0;
	virtual void enableExti(bool enable = true) = 0;

};

class IIOPinsPool
{
public:
	virtual ~IIOPinsPool() {}
	virtual IIOPin* getIOPin(uint8_t portNumber, uint8_t pinNumber) = 0;
	virtual void resetAllPins() = 0;
};

extern IIOPinsPool* IOPins;

#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_IO_PINS_HPP_ */

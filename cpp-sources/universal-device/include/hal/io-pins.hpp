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

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_IO_PINS_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HAL_IO_PINS_HPP_

#include <stdint.h>
#include <functional>

using IOPinCallback = std::function<void(bool)>;

class IIOPin
{
public:
	constexpr static uint8_t PORTA = 0;
	constexpr static uint8_t PORTB = 1;
	constexpr static uint8_t PORTC = 2;
	constexpr static uint8_t PORTD = 3;
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

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


#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_IO_PINS_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_IO_PINS_HPP_

#include "hal/io-pins.hpp"
#include "stm32f1xx_hal.h"
#include "core/os-wrappers.hpp"

#include <map>

class IOPin : public IIOPin
{
public:
	IOPin(uint8_t portNumber, uint8_t pinNumber);

	bool state();

	/// Switch to state "1"
	void set();
	/// Switch to state "0"
	void reset();

	void switchToOutput();
	void switchToInput();

	void setExtiCallback(const IOPinCallback& callback, bool direclyFromISR = false);
	void enableExti(bool enable = true);
	void extiInterrupt();

	static GPIO_TypeDef* parsePortNumber(uint8_t portNumber);
	static uint16_t pinNumberToMask(uint8_t pinNumber);
	static uint8_t maskToPinNumber(uint16_t pinMask);
private:
	IOPinCallback m_callback = nullptr;
	GPIO_TypeDef* m_gpio;
	uint16_t m_pinMask;
	bool m_extiEnabled = false;
	bool isOutputMode = false;
	bool m_runDirectlyFromISR = false;
};

class IOPinsPool : public IIOPinsPool
{
friend class IOPin;
public:
	IOPinsPool();
	IIOPin* getIOPin(uint8_t portNumber, uint8_t pinNumber);
	void resetAllPins();

private:
	using QueueCallback = std::function<void(void)>;
	void createIOPin(uint8_t portNumber, uint8_t pinNumber);
	std::map<std::pair<uint8_t, uint8_t>, IIOPin*> m_interfaces;
	Worker m_callbackCaller{10};
};





#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_IO_PINS_HPP_ */

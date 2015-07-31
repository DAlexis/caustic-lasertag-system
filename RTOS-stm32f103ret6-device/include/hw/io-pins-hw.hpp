/*
 * io-pins.hpp
 *
 *  Created on: 20 июня 2015 г.
 *      Author: alexey
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
	TaskDeferredFromISR m_deferredTask;
	IOPinCallback m_callback = nullptr;
	GPIO_TypeDef* m_gpio;
	uint16_t m_pinMask;
	bool m_extiEnabled = false;
	bool isOutputMode = false;
	bool m_runDirectlyFromISR = false;
};

class IOPinsPool : public IIOPinsPool
{
public:
	IOPinsPool();
	IIOPin* getIOPin(uint8_t portNumber, uint8_t pinNumber);

private:
	void createIOPin(uint8_t portNumber, uint8_t pinNumber);
	std::map<std::pair<uint8_t, uint8_t>, IIOPin*> m_interfaces;
};





#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_HW_IO_PINS_HPP_ */

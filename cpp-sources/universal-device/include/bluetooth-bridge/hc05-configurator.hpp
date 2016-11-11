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

#ifndef UNIVERSAL_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_HC05_CONFIGURATOR_HPP_
#define UNIVERSAL_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_HC05_CONFIGURATOR_HPP_

#include "hal/io-pins.hpp"
#include "hal/uart.hpp"

class HC05Configurator
{
public:
	constexpr static uint32_t uartDefaultSpeed = 9600;
	constexpr static uint32_t uartTargetSpeed = 115200;

	enum class HC05Result {
		waiting,
		failed,
		timeout,
		ok,
		notRunned
	};

	void init(IIOPin* key, IUARTManager* uart);
	HC05Result selectSpeed();
	HC05Result test();

	void configure();
	IUARTManager* getUART();
	void enterAT();
	void leaveAT();
	static const char* parseResult(HC05Result result);

private:

	void waitForResult();
	void sendATCommand(const char* command, uint16_t size);

	void rxCallbackResult(uint8_t* data, uint16_t size);

	IIOPin* m_key = nullptr;
	IUARTManager* m_uart = nullptr;

	HC05Result m_result = HC05Result::notRunned;
	char m_response[100];
};



#endif /* UNIVERSAL_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_HC05_CONFIGURATOR_HPP_ */

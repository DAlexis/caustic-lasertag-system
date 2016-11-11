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

#include "bluetooth-bridge/hc05-configurator.hpp"
#include "hal/system-clock.hpp"
#include "core/logging.hpp"
#include <string.h>

void HC05Configurator::init(IIOPin* key, IUARTManager* uart)
{
	m_key = key;
	m_key->switchToOutput();
	m_uart = uart;
	m_uart->enableStopChar(true);
	m_uart->setStopChar('\n');
	m_uart->setBlockSize(100);
	m_uart->setRXDoneCallback([this](uint8_t* data, uint16_t size) { rxCallbackResult(data, size); });
}

void HC05Configurator::enterAT()
{
	m_key->set();
	systemClock->wait_us(10000);
}

void HC05Configurator::leaveAT()
{
	m_key->reset();
}

void HC05Configurator::waitForResult()
{
	Time time = systemClock->getTime();
	while (m_result == HC05Result::waiting)
	{
		if (systemClock->getTime() - time > 100000)
		{
			m_result = HC05Result::timeout;
			break;
		}
	}
}

void HC05Configurator::sendATCommand(const char* command, uint16_t size)
{
	m_result = HC05Result::waiting;
	m_response[0] = '\0';
	m_uart->transmitSync((uint8_t*) command, size);
}

HC05Configurator::HC05Result HC05Configurator::test()
{
	enterAT();
	const char* at="AT\r\n";
	sendATCommand(at, sizeof(at));
	waitForResult();
	return m_result;
}

HC05Configurator::HC05Result HC05Configurator::selectSpeed()
{
	// Trying default:
	m_uart->init(uartDefaultSpeed);
	HC05Result result = test();
	if (result != HC05Result::ok)
	{
		debug << "Default HC-05 speed rejected due to error while testing: " << parseResult(result);
		m_uart->init(uartTargetSpeed);
		HC05Result result = test();
		if (result != HC05Result::ok)
		{
			error << "Cannot determine usart speed for HC-05!";
		}
	}

	return result;
}

void HC05Configurator::configure()
{
	enterAT();
	const char* atName="AT+NAME=CausticBridge2\r\n";
	const char* atUart="AT+UART=115200,1,0\r\n";

	systemClock->wait_us(10000);
	sendATCommand(atUart, strlen(atUart));
	waitForResult();
	if (m_result != HC05Result::ok)
	{
		error << "Cannot set UART speed for module: " << parseResult(m_result) << " resp: " << m_response;
		//return;
	} else {
		info << "UART Speed set properly!";
	}
	systemClock->wait_us(10000);
	sendATCommand(atName, strlen(atName));
	waitForResult();
	if (m_result != HC05Result::ok)
	{
		error << "Cannot set name for module: " << parseResult(m_result) << " resp: " << m_response;
		//return;
	} else {
		info << "Name set properly!";
	}

	systemClock->wait_us(10000);
}

IUARTManager* HC05Configurator::getUART()
{
	return m_uart;
}

const char* HC05Configurator::parseResult(HC05Configurator::HC05Result result)
{
	switch(result)
	{
	case HC05Result::waiting: return "waiting";
	case HC05Result::failed: return "failed";
	case HC05Result::timeout: return "timeout";
	case HC05Result::ok: return "ok";
	case HC05Result::notRunned: return "notRunned";
	default: return "Invalid result";
	}
}

void HC05Configurator::rxCallbackResult(uint8_t* data, uint16_t size)
{
	memcpy(m_response, data, size);
	m_response[size] = '\0';
	if (size >= 2 && data[0] == 'O' && data[1] == 'K')
	{
		m_result = HC05Result::ok;
	} else {
		m_result = HC05Result::failed;
	}
}

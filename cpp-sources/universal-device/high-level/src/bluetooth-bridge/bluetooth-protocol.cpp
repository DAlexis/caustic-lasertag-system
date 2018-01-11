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


#include "bluetooth-bridge/bluetooth-protocol.hpp"
#include <string.h>

using namespace Bluetooth;

/////////////////////
// Message
Message::Message()
{
	memset(payload, 0, sizeof(payload[0])*payloadMaxLen);
}

uint8_t Message::payloadLength()
{
	return length - headerLength;
}

void Message::print()
{
	trace << "+- Bluetooth message:";
	trace << "|- address: " << ADDRESS_TO_STREAM(address);
	trace << "|- length: " << length;
	trace << "|- checksum: " << checksum;
	trace << "`- payload: ";
	trace << hexStr(payload, length-headerLength);
}

void Message::setChecksum()
{
	checksum = getChecksum();
}

uint8_t Message::getChecksum()
{
	uint8_t result = 0;
	result += length;
	result += address.address[0];
	result += address.address[1];
	result += address.address[2];
	for (int i=0; i<payloadLength(); i++)
		result += payload[i];
	return result;
}

bool Message::isChecksumCorrect()
{
	return checksum == getChecksum();
}


/////////////////////
// MessageCreator
void MessageCreator::setSender(const DeviceAddress&& sender)
{
	m_message.address = sender;
}

bool MessageCreator::addData(uint8_t size, const uint8_t* data)
{
	if (maxMessageLen - m_message.length < size)
		return false;

	memcpy(&(m_message.payload[m_message.length-m_message.headerLength]), data, size);
	m_message.length += size;

	return true;
}

void MessageCreator::clear()
{
	m_message.length = m_message.headerLength;
}

MessageReceiver::MessageReceiver()
{
	reset();
}

void MessageReceiver::readByte(uint8_t byte)
{
	uint8_t* p = reinterpret_cast<uint8_t*>(&m_message);

	Time now = systemClock->getTime();

	if (now - m_lastReceive > timeout)
	{
		reset();
	}

	if (m_offset == sizeof(Message))
	{
		m_lastReceive = now;
		return;
	}

	m_lastReceive = now;
	p[m_offset++] = byte;
}

void MessageReceiver::reset()
{
	m_offset = 0;
}

Message& MessageReceiver::get()
{
	return m_message;
}

bool MessageReceiver::ready()
{
	return (m_offset == m_message.length);
}

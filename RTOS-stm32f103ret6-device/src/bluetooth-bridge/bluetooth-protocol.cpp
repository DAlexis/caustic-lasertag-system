/*
 * bluetooth-protocol.cpp
 *
 *  Created on: 26 сент. 2015 г.
 *      Author: alexey
 */

#include "bluetooth-bridge/bluetooth-protocol.hpp"
#include <string.h>

using namespace Bluetooth;

void MessageCreator::setSender(const DeviceAddress&& sender)
{
	m_message.address = sender;
}

bool MessageCreator::addData(uint8_t size, const uint8_t* data)
{
	if (maxMessageLen - m_message.length < size)
		return false;

	memcpy(&(m_message.data[m_message.length-m_message.headerLength]), data, size);
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

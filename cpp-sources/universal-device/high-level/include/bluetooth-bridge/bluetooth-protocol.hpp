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

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_BLUETOOTH_PROTOCOL_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_BLUETOOTH_PROTOCOL_HPP_

#include "network/network-base-types.hpp"
#include "rcsp/RCSP-base-types.hpp"
#include "rcsp/base-types.hpp"
#include "core/logging.hpp"
#include "core/string-utils.hpp"
#include "hal/system-clock.hpp"

namespace Bluetooth
{

	constexpr static uint8_t maxMessageLen = 50;

	#pragma pack(push, 1)
		struct Message
		{
			constexpr static uint8_t headerLength =
					sizeof(uint8_t) + sizeof(uint8_t) + sizeof(DeviceAddress);
			constexpr static uint8_t payloadMaxLen = maxMessageLen - headerLength;

			Message();
			uint8_t payloadLength();
			void print();
			void setChecksum();
			uint8_t getChecksum();
			bool isChecksumCorrect();

			uint8_t length = headerLength;
			uint8_t checksum = 0;
			DeviceAddress address;
			uint8_t payload[payloadMaxLen];
		};
	#pragma pack(pop)

	class MessageCreator
	{
	public:
		void setSender(const DeviceAddress&& sender);
		bool addData(uint8_t size, const uint8_t* data);
		void clear();

		uint8_t size() { return m_message.length; }
		void* data() { return &m_message; }
		Message& msg() {return m_message; }

	private:
		Message m_message;
	};

	class MessageReceiver
	{
	public:
		constexpr static TimeInterval timeout = 1000;
		MessageReceiver();
		void reset();
		void readByte(uint8_t byte);
		Message& get();
		bool ready();

	private:
		Message m_message;
		uint16_t m_offset = 0;
		Time m_lastReceive = 0;
	};
}

#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_BLUETOOTH_PROTOCOL_HPP_ */

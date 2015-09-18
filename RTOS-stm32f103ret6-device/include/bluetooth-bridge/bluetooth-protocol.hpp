/*
 * bluetooth-protocol.hpp
 *
 *  Created on: 13 сент. 2015 г.
 *      Author: alexey
 */

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_BLUETOOTH_PROTOCOL_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_BLUETOOTH_PROTOCOL_HPP_

#include "network/network-base-types.hpp"

class BluetoothMessageCreator
{
public:
	constexpr static uint8_t maxMessageLen = 50;
	void setSender(const DeviceAddress&& sender);
	bool addData(uint8_t size, const uint8_t* data);
	void clear();

	uint8_t size() { return m_message.length; }
	void* data() { return &m_message; }


#pragma pack(push, 1)
	struct Message
	{
		constexpr static uint8_t headerLength = sizeof(uint8_t) + sizeof(DeviceAddress);
		uint8_t length = headerLength;
		DeviceAddress address;
		uint8_t data[maxMessageLen - sizeof(length)];
	};
#pragma pack(pop)

private:



	Message m_message;
};




#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_BLUETOOTH_BRIDGE_BLUETOOTH_PROTOCOL_HPP_ */

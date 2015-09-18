/*
 * bluetooth-bridge.cpp
 *
 *  Created on: 7 сент. 2015 г.
 *      Author: alexey
 */

#include "bluetooth-bridge/bluetooth-bridge.hpp"
#include "network/network-layer.hpp"
#include "network/broadcast.hpp"
#include "core/logging.hpp"
#include <string.h>

AnyBuffer::AnyBuffer(uint16_t _size, const void *_data) :
	size(_size)
{
	data = nullptr;
	data = new uint8_t[size];
	if (data != nullptr && _data != nullptr)
		memcpy(data, _data, size);
}

void BluetoothMessageCreator::setSender(const DeviceAddress&& sender)
{
	m_message.address = sender;
}

bool BluetoothMessageCreator::addData(uint8_t size, const uint8_t* data)
{
	if (maxMessageLen - m_message.length < size)
		return false;

	memcpy(&(m_message.data[m_message.length-m_message.headerLength]), data, size);
	m_message.length += size;

	return true;
}

void BluetoothMessageCreator::clear()
{
	m_message.length = m_message.headerLength;
}

void BluetoothBridge::init(const Pinout& pinout)
{
	deviceConfig.devAddr.address[0] = 50;
	deviceConfig.devAddr.address[1] = 50;
	deviceConfig.devAddr.address[2] = 50;

	NetworkLayer::instance().setAddress(deviceConfig.devAddr);
	NetworkLayer::instance().setPackageReceiver(
		[this](DeviceAddress sender, uint8_t* payload, uint16_t payloadLength)
		{
			receiveNetworkPackage(sender, payload, payloadLength);
		}
	);
	NetworkLayer::instance().registerBroadcast(broadcast.any);
	NetworkLayer::instance().registerBroadcast(broadcast.bluetoothBridges);
	NetworkLayer::instance().init();



	m_bluetoothPort = UARTSFactory->create();
	m_bluetoothPort->setRXDoneCallback(
		[this](uint8_t* buffer, uint16_t size)
		{
			receiveBluetoothPackageISR(buffer, size);
		}
	);
	m_bluetoothPort->init(2, 115200);
	/*
	m_bluetoothPort->setRXDoneCallback(
			[this](uint8_t* buffer, uint16_t size)
			{
				memcpy(m_tmpBuffer, buffer, size);
				m_tmpBuffer[size] = '\0';
				m_worker.addFromISR(
						[this]()
						{
							info << "Incoming: " << m_tmpBuffer;
						}
				);

			}
	);*/

	m_workerToBluetooth.setStackSize(256);
	m_workerToBluetooth.run();
	m_workerToNetwork.setStackSize(256);
	m_workerToNetwork.run();
}

void BluetoothBridge::setDafaultPinout(Pinout& pinout)
{
}

bool BluetoothBridge::checkPinout(const Pinout& pinout)
{
	return true;
}

void BluetoothBridge::receiveNetworkPackage(DeviceAddress sender, uint8_t* payload, uint16_t payloadLength)
{
	debug << "Processing incoming network package";
	m_bluetoothMsgCreator.clear();
	m_bluetoothMsgCreator.setSender(std::move(sender));
	m_bluetoothMsgCreator.addData(payloadLength, payload);
	AnyBuffer* msgBuffer = new AnyBuffer(m_bluetoothMsgCreator.size(), m_bluetoothMsgCreator.data());
	m_workerToBluetooth.add(
		[this, msgBuffer] ()
		{
			sendBluetoothMessage(msgBuffer);
			delete msgBuffer;
		}
	);
}

void BluetoothBridge::receiveBluetoothPackageISR(uint8_t* buffer, uint16_t size)
{
	AnyBuffer* msgBuffer = new AnyBuffer(size, buffer);
	m_workerToNetwork.addFromISR(
		[this, msgBuffer] ()
		{
			sendNetworkPackage(msgBuffer);
			delete msgBuffer;
		}
	);
}

void BluetoothBridge::sendBluetoothMessage(AnyBuffer* buffer)
{
	debug << "Sending bluetooth message";
	// Transmitting to bluetooth module and waiting while transmit is done
	m_bluetoothPort->transmit(buffer->data, buffer->size);
	while (m_bluetoothPort->txBusy())
		Kernel::yeld();
}

void BluetoothBridge::sendNetworkPackage(AnyBuffer* buffer)
{
	debug << "Sending package to network";
	// Dispatching bluetooth message
	BluetoothMessageCreator::Message *bluetoothMessage = reinterpret_cast<BluetoothMessageCreator::Message *>(buffer->data);

	// Sending message body as is
	NetworkLayer::instance().send(bluetoothMessage->address, bluetoothMessage->data, bluetoothMessage->length, true);
}

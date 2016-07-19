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


#include "bluetooth-bridge/bluetooth-bridge.hpp"
#include "network/network-layer.hpp"
#include "network/broadcast.hpp"
#include "core/logging.hpp"
#include "core/string-utils.hpp"
#include <string.h>

using namespace Bluetooth;

BluetoothBridgePackageTimings bluetoothBridgePackageTimings;

AnyBuffer::AnyBuffer(uint16_t _size, const void *_data) :
	size(_size)
{
	data = nullptr;
	data = new uint8_t[size];
	if (data != nullptr && _data != nullptr)
		memcpy(data, _data, size);
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
	//NetworkLayer::instance().enableRegularNRFReinit();


	m_bluetoothPort = UARTSFactory->create();
	m_bluetoothPort->setBlockSize(1);
	m_bluetoothPort->setRXDoneCallback(
		[this](uint8_t* buffer, uint16_t size)
		{
			UNUSED_ARG(size);
			receiveBluetoothOneByteISR(buffer[0]);
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
	trace << "Bluetooth message to be sent: ";
	printHex(msgBuffer->data, msgBuffer->size);
	m_workerToBluetooth.add(
		[this, msgBuffer] ()
		{
			sendBluetoothMessage(msgBuffer);
			systemClock->wait_us(1000);
			delete msgBuffer;
		}
	);
}

void BluetoothBridge::receiveBluetoothOneByteISR(uint8_t byte)
{
	m_receiver.readByte(byte);
	if (m_receiver.ready())
	{
		//printf("R\n");
		receiveBluetoothPackageISR(reinterpret_cast<uint8_t*>(&m_receiver.get()), m_receiver.get().length);
		m_receiver.reset();
	}
}

void BluetoothBridge::receiveBluetoothPackageISR(uint8_t* buffer, uint16_t size)
{
	AnyBuffer* msgBuffer = new AnyBuffer(size, buffer);
	m_workerToNetwork.addFromISR(
		[this, msgBuffer] ()
		{
			info << "Incoming bluetooth: ";
			printHex(msgBuffer->data, msgBuffer->size);
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
	Message *bluetoothMessage = reinterpret_cast<Message *>(buffer->data);

	debug << "Bluetooth message for " << ADDRESS_TO_STREAM(bluetoothMessage->address);
	// Sending message body as is
	if (broadcast.isBroadcast(bluetoothMessage->address))
	{
		// We need special timings for broadcasts
		NetworkLayer::instance().send(
				bluetoothMessage->address,
				bluetoothMessage->data,
				bluetoothMessage->payloadLength(),
				true,
				nullptr,
				bluetoothBridgePackageTimings.broadcast
		);
	} else {
		// Not broadcast packages are with default timings
		NetworkLayer::instance().send(bluetoothMessage->address, bluetoothMessage->data, bluetoothMessage->payloadLength(), true);
	}

}

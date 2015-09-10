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

void BluetoothBridge::init(const Pinout& pinout)
{
	NetworkLayer::instance().setAddress(deviceConfig.devAddr);
	NetworkLayer::instance().setPackageReceiver(
			[this](DeviceAddress sender, uint8_t* payload, uint16_t payloadLength)
			{
				receivePackage(sender, payload, payloadLength);
			}
	);
	NetworkLayer::instance().registerBroadcast(broadcast.any);
	NetworkLayer::instance().registerBroadcast(broadcast.bluetoothBridges);
	NetworkLayer::instance().init();

	m_worker.setStackSize(256);
	m_worker.run();

	m_bluetoothPort = UARTSFactory->create();
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
	);
	m_bluetoothPort->init(2, 9600);
}

void BluetoothBridge::setDafaultPinout(Pinout& pinout)
{
}

bool BluetoothBridge::checkPinout(const Pinout& pinout)
{
	return true;
}

void BluetoothBridge::receivePackage(DeviceAddress sender, uint8_t* payload, uint16_t payloadLength)
{

}

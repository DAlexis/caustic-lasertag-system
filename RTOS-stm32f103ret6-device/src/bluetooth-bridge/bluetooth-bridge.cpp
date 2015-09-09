/*
 * bluetooth-bridge.cpp
 *
 *  Created on: 7 сент. 2015 г.
 *      Author: alexey
 */

#include "bluetooth-bridge/bluetooth-bridge.hpp"
#include "network/network-layer.hpp"
#include "network/broadcast.hpp"


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

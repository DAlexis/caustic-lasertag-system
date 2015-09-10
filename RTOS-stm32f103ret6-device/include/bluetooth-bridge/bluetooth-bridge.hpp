/*
 * bluetooth-bridge.cpp
 *
 *  Created on: 7 сент. 2015 г.
 *      Author: alexey
 */

#include "core/device-initializer.hpp"
#include "device/device.hpp"
#include "hal/uart.hpp"
#include "core/os-wrappers.hpp"


class BluetoothBridge : public IAnyDevice
{
public:
	void init(const Pinout& pinout);
	void setDafaultPinout(Pinout& pinout);
	bool checkPinout(const Pinout& pinout);

	DeviceConfiguration deviceConfig;

private:
	void receivePackage(DeviceAddress sender, uint8_t* payload, uint16_t payloadLength);

	IUARTManager* m_bluetoothPort;
	Worker m_worker{5};
	char m_tmpBuffer[200];
};

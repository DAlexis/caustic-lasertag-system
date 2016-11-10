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
#include "core/device-initializer.hpp"
#include "device/device.hpp"
#include "hal/uart.hpp"
#include "core/os-wrappers.hpp"

struct AnyBuffer
{
	AnyBuffer(uint16_t _size, const void *_data = nullptr);
	uint8_t* data;
	const uint16_t size;
};

class BluetoothBridgePackageTimings
{
public:
	PackageTimings broadcast{false, 1000000, 100000, 100000};
};

extern BluetoothBridgePackageTimings bluetoothBridgePackageTimings;

class BluetoothBridge : public IAnyDevice
{
public:
	BluetoothBridge();
	void init(const Pinout& pinout);
	void setDafaultPinout(Pinout& pinout);
	bool checkPinout(const Pinout& pinout);

	DeviceConfiguration deviceConfig;

private:
	constexpr static uint16_t bluetoothIncommingBufferSize = 200;
	void receiveNetworkPackage(const DeviceAddress sender, uint8_t* payload, uint16_t payloadLength);
	void receiveBluetoothOneByteISR(uint8_t byte);
	void receiveBluetoothPackageISR(uint8_t* buffer, uint16_t size);

	void sendBluetoothMessage(AnyBuffer* buffer);
	void sendNetworkPackage(AnyBuffer* buffer);

	Bluetooth::MessageCreator m_bluetoothMsgCreator;

	IUARTManager* m_bluetoothPort = nullptr;

	/// @todo Queues should be as large as possible, so need to increase its size
	Worker m_workerToBluetooth{30};
	Worker m_workerToNetwork{30};
	Bluetooth::MessageReceiver m_receiver;
	TasksPool m_tasksPool{"BtBrPool"};
};

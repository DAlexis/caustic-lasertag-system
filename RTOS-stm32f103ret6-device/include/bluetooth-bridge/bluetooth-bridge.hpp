/*
 * bluetooth-bridge.cpp
 *
 *  Created on: 7 сент. 2015 г.
 *      Author: alexey
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

class BluetoothBridge : public IAnyDevice
{
public:
	void init(const Pinout& pinout);
	void setDafaultPinout(Pinout& pinout);
	bool checkPinout(const Pinout& pinout);

	DeviceConfiguration deviceConfig;

private:
	constexpr static uint16_t bluetoothIncommingBufferSize = 200;
	void receiveNetworkPackage(DeviceAddress sender, uint8_t* payload, uint16_t payloadLength);
	void receiveBluetoothOneByteISR(uint8_t byte);
	void receiveBluetoothPackageISR(uint8_t* buffer, uint16_t size);

	void sendBluetoothMessage(AnyBuffer* buffer);
	void sendNetworkPackage(AnyBuffer* buffer);

	Bluetooth::MessageCreator m_bluetoothMsgCreator;

	IUARTManager* m_bluetoothPort;
	Worker m_workerToBluetooth{10};
	Worker m_workerToNetwork{10};
	Bluetooth::MessageReceiver m_receiver;
};

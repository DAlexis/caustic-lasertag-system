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

#include <any-device/device.hpp>
#include "any-device/any-onc-device-base.hpp"
#include "bluetooth-bridge/bluetooth-protocol.hpp"
#include "bluetooth-bridge/hc05-configurator.hpp"
#include "core/device-initializer.hpp"
#include "core/diagnostic.hpp"
#include "hal/uart.hpp"
#include "core/os-wrappers.hpp"
#include "network/network-client.hpp"
#include "utils/memory.hpp"

#include <queue>

class BluetoothBridgePackageTimings
{
public:
	PackageTimings broadcast{false, 1000000, 100000, 100000};
};

extern BluetoothBridgePackageTimings bluetoothBridgePackageTimings;

class BluetoothBridge : public AnyONCDeviceBase, public IPayloadReceiver
{
public:
	BluetoothBridge(INetworkLayer *existingNetworkLayer = nullptr);
	void initAsSecondaryDevice(const Pinout& pinout, bool isSdcardOk);
	void init(const Pinout& pinout, bool isSdcardOk) override;
	void setDefaultPinout(Pinout& pinout) override;
	bool checkPinout(const Pinout& pinout) override;

	BluetoothBridgeConfiguration config{m_aggregator};

private:
	constexpr static uint16_t queuesSizeMax = 30;

	void receive(DeviceAddress sender, const uint8_t* payload, uint16_t payloadLength) override;

	void configureBluetooth();
	void receiveNetworkPackage(const DeviceAddress sender, uint8_t* payload, uint16_t payloadLength);
	void receiveBluetoothOneByteISR(uint8_t byte);
	void receiveBluetoothPackageISR(Bluetooth::Message* msg);

	void sendBluetoothMessage(Bluetooth::Message* msg);
	void sendNetworkPackage(DeviceAddress addr, uint8_t* payload, uint16_t size);
    /**
     * Main loop function that works with queues
     */
	void mainLoop();
	bool hasFreeSpaceInQueues();

	Bluetooth::MessageCreator m_bluetoothMsgCreator;
	HC05Configurator m_configurator{deviceConfig.deviceName, config.bluetoothPin};

	IUARTManager* m_bluetoothPort = nullptr;

	Mutex m_messagesToBluetoothMutex;
	std::queue<Bluetooth::Message*> m_messagesToBluetooth;
	std::queue<Bluetooth::Message*> m_messagesToNetwork;
	TaskOnce m_toBluetoothTask{ [this](){ mainLoop(); }, 512 };
	//TaskOnce m_toNetworkTask{ [this](){ toNetworkTask(); }, 512 };

	/// @todo Queues should be as large as possible, so need to increase its size
	Worker m_workerToNetwork{30};
	Bluetooth::MessageReceiver m_receiver;

	TasksPool m_tasksPool{"BtBrPool"};
	Mutex m_processingMutex;

	Stager m_bbStager{"BBStager"};
};

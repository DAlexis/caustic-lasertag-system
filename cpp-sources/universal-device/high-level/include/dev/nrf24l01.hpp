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

#ifndef RTOS_STM32F103RET6_DEVICE_INCLUDE_DEV_NRF24L01_HPP_
#define RTOS_STM32F103RET6_DEVICE_INCLUDE_DEV_NRF24L01_HPP_

#include "network/radio-physical-interface.hpp"
#include "hal/spi.hpp"
#include "hal/io-pins.hpp"
#include "hal/system-clock.hpp"
#include "core/diagnostic.hpp"

#define RADIO_ADDRESS_SIZE  5

using DataReceiveCallback = std::function<void(uint8_t/* channel*/, uint8_t*/* data*/)>;
using TXMaxRetriesCallback = std::function<void(void)>;
using TXDoneCallback = std::function<void(void)>;

class NRF24L01Manager : public IRadioPhysicalDevice
{
public:
    enum EnableDisable
    {
        DISABLE_OPTION = 0,
        ENABLE_OPTION = 1,
    };

    constexpr static unsigned int payloadSize = 32;
    constexpr static uint8_t defaultRadioChannel = 1;

    NRF24L01Manager();

    UintParameter getPayloadSize() override;

    void setDataReceiveCallback(DataReceiveCallback callback) override;
    void setTXDoneCallback(TXDoneCallback callback) override;
	void sendData(uint8_t size, uint8_t* data) override;
    void setRXAddress(uint8_t channel, uint8_t* address) override;
    void setTXAddress(uint8_t* address) override;
    void printStatus() override;

    void init(
    		IIOPin* chipEnablePin,
			IIOPin* chipSelectPin,
			IIOPin* IRQPin,
			uint8_t SPIIndex,
			bool useInterrupts,
			uint8_t radioChannel = defaultRadioChannel
	);

    void setTXMaxRetriesCallback(TXMaxRetriesCallback callback);
    void resetAllIRQ();

    void interrogate();

    // EN_RXARRD
    void enablePipe(uint8_t pipe, uint8_t value);

    void flushTX();
    void flushRX();

    void enableDebug(bool debug = true);

private:
    constexpr static Time reinitPeriod = 10000000;

    enum Power
	{
		POWER_OFF = 0,
		POWER_ON = 1
	};

	enum InterruptionsMasks
	{
		IM_MASK_MAX_RETRIES = 1,
		IM_MASK_TX_DATA_READY  = 2,
		IM_MASK_RX_DATA_READY  = 4
	};

	enum CRCEnableDisable
	{
		CRC_DISABLE = 0,
		CRC_ENABLE = 1
	};

	enum CRCLen
	{
		CRC1BYTE = 0,
		CRC2BYTES = 1,
	};

	enum AdressWidth
	{
		AW_3BYTES,
		AW_4BYTES,
		AW_5BYTES,
	};

	enum Baudrate
	{
		BR_1MBIT = 0,
		BR_2MBIT = 1,
	};

	enum TXPower
	{
		PW_m18DB = 0,
		PW_m12DB = 1,
		PW_m6DB = 2,
		PW_0DB = 3,
	};

	enum LNAGain
	{
		LNA_DISABLE = 0,
		LNA_ENABLE = 1,
	};

	enum RXTXMODE
	{
		MODE_TRANSMITTER = 0,
		MODE_RECEIVER = 1,
	};

	constexpr static uint16_t timeEnoughForTransmission = 1000;

	void switchToTX();
	void switchToRX();

	void receiveData(uint8_t size, uint8_t* data);

	void initInterrupts();
	void writeReg(uint8_t reg, uint8_t size, uint8_t *data);
	void readReg(uint8_t reg, uint8_t size, uint8_t *data);

	void extiHandler(bool state);

	// CONFIG register
	void setConfig(uint8_t interruptionsMask,
			uint8_t enableCRC,
			uint8_t CRC2bytes,
			uint8_t powerUP,
			uint8_t isRecieving);

	void switchToReceiver();
	void switchToTranmitter();

	// STATUS register parcers
	void updateStatus();
	inline bool isRXDataReady();
	inline bool isTXDataSent();
	inline bool isMaxRetriesReached();
	inline int getPipeNumberAvaliableForRXFIFO();
	inline bool isTXFIFOFull();

	void resetRXDataReady();
	void resetTXDataSent();
	void resetMaxRetriesReached();

	// CD register
	bool isCarrierDetected();

	// EN_AA register
	void setAutoACK(uint8_t channel, bool value);

	// SETUP_AW
	void setAdressWidth(AdressWidth width);

	// SETUP_RETR
	void setupRetransmission(uint8_t delay, uint8_t count);

	// RF_CH
	void setRFChannel(uint8_t number);
	void clearLostPackagesCount();

	// RF_SETUP
	void setRFSettings(uint8_t use2MBits, uint8_t power, uint8_t LNAGain);

	// OBSERVE_TX
	uint8_t getLostPackagesCount();
	uint8_t getResentPackagesCount();

	// RX_ADDR_Pn
	void readRXAdresses();

	// TX_ADDR
	void readTXAdress();

	// RX_PW_Pn
	void setRXPayloadLength(uint8_t channel, uint8_t payloadLength);

	// FIFO_STATUS
	bool isReuseEnabled();
	bool isTXFull();
	bool isTXEmpty();
	bool isRXFull();
	bool isRXEmpty();

	// Wires
	void chipEnableOn();
	void chipEnableOff();
	void chipSelect();
	void chipDeselect();
	void CEImpulse();

	void onRXDataReady();
	void onTXDataSent();
	void onMaxRetriesReached();

	void reinitIfNeeded();

	IIOPin* m_chipEnablePin = nullptr;
	IIOPin* m_chipSelectPin = nullptr;
	IIOPin* m_IRQPin = nullptr;

	uint8_t m_SPIIndex = 0;
	uint8_t m_radioChannel = 1;

	ISPIManager* m_spi = nullptr;

	bool m_useInterrupts = false;

	bool m_needInterrogation = false;

	uint8_t m_status = 0;
	uint8_t m_config = 0;
	uint8_t m_RFChannel = defaultRadioChannel;
	uint8_t m_TXAdress[RADIO_ADDRESS_SIZE];
	uint8_t m_RXAdressP0[RADIO_ADDRESS_SIZE];
	uint8_t m_RXAdressP1[RADIO_ADDRESS_SIZE];
	uint8_t m_RXAdressP2;
	uint8_t m_RXAdressP3;
	uint8_t m_RXAdressP4;
	uint8_t m_RXAdressP5;

	DataReceiveCallback m_RXcallback = nullptr;
	TXMaxRetriesCallback m_TXMaxRTcallback = nullptr;
	TXDoneCallback m_TXDoneCallback = nullptr;

	Time m_lastTransmissionTime = 0;
	bool m_waitingForTransmissionEnd = false;

	bool m_debug = false;

	Time m_lastReinitTime = 0;

	Stager m_stager{"NRF24L01Manager", 1000000};
};




#endif /* RTOS_STM32F103RET6_DEVICE_INCLUDE_DEV_NRF24L01_HPP_ */

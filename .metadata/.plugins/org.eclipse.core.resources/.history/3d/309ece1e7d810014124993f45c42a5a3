/*
 * radio.hpp
 *
 *  Created on: 19 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_RADIO_HPP_
#define LAZERTAG_RIFLE_INCLUDE_RADIO_HPP_

#include "spi.hpp"

#define RADIO_ADDRESS_SIZE  5
#define PAYLOAD_SIZE        5
#define RADIO_CHANNEL       1

using DataReceiveCallback = void (*) (void* object, unsigned char channel, unsigned char* data);
using TXMaxRetriesCallback = void (*) (void* object);
using TXDoneCallback = void (*) (void* object);

class RadioManager
{
public:
    enum EnableDisable
    {
        DISABLE_OPTION = 0,
        ENABLE_OPTION = 1,
    };

    RadioManager();
    void init();
    void printStatus();
    void setDataReceiveCallback(DataReceiveCallback callback, void* object);
    void setTXMaxRetriesCallback(TXMaxRetriesCallback callback, void* object);
    void setTXDoneCallback(TXDoneCallback callback, void* object);
    void sendData(unsigned char size, unsigned char* data);

    void resetAllIRQ();
    void setRXAddress(unsigned char channel, unsigned char* address);
    void setTXAddress(unsigned char* address);

    void interruptHandler();

    // EN_RXARRD
    void enablePipe(unsigned char pipe, unsigned char value);

    void flushTX();
    void flushRX();

private:
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

    void switchToTX();
    void switchToRX();

    void receiveData(unsigned char size, unsigned char* data);

    void initInterrupts();
    void writeReg(unsigned char reg, unsigned char size, unsigned char *data);
    void readReg(unsigned char reg, unsigned char size, unsigned char *data);


    // CONFIG register
    void setConfig(unsigned char interruptionsMask,
            unsigned char enableCRC,
            unsigned char CRC2bytes,
            unsigned char powerUP,
            unsigned char isRecieving);

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
    void setAutoACK(unsigned char channel, bool value);

    // SETUP_AW
    void setAdressWidth(AdressWidth width);

    // SETUP_RETR
    void setupRetransmission(unsigned char delay, unsigned char count);

    // RF_CH
    void setRFChannel(unsigned char number);
    void clearLostPackagesCount();

    // RF_SETUP
    void setRFSettings(unsigned char use2MBits, unsigned char power, unsigned char LNAGain);

    // OBSERVE_TX
    unsigned char getLostPackagesCount();
    unsigned char getResentPackagesCount();

    // RX_ADDR_Pn
    void readRXAdresses();

    // TX_ADDR
    void readTXAdress();

    // RX_PW_Pn
    void setRXPayloadLength(unsigned char channel, unsigned char payloadLength);

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
    unsigned char m_status;

    unsigned char m_config;
    unsigned char m_RFChannel;
    unsigned char m_TXAdress[RADIO_ADDRESS_SIZE];
    unsigned char m_RXAdressP0[RADIO_ADDRESS_SIZE];
    unsigned char m_RXAdressP1[RADIO_ADDRESS_SIZE];
    unsigned char m_RXAdressP2;
    unsigned char m_RXAdressP3;
    unsigned char m_RXAdressP4;
    unsigned char m_RXAdressP5;

    DataReceiveCallback m_RXcallback;
    void* m_RXcallbackObject;
    TXMaxRetriesCallback m_TXMaxRTcallback;
    void* m_TXMaxRTcallbackObject;
    TXDoneCallback m_TXDoneCallback;
    void* m_TXDoneCallbackObject;
};

extern RadioManager radio;


#endif /* LAZERTAG_RIFLE_INCLUDE_RADIO_HPP_ */


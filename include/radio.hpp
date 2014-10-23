/*
 * radio.hpp
 *
 *  Created on: 19 окт. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_RADIO_HPP_
#define LAZERTAG_RIFLE_INCLUDE_RADIO_HPP_

#include "spi.hpp"

class RadioManager
{
public:
    RadioManager();
    void init();
    void readTXAdress();
    void writeTXAdress();
    void printStatus();
    void testTX();
    void sendData(unsigned char size, unsigned char* data);
    void receiveData(unsigned char size, unsigned char* data);
    void resetAllIRQ();
//private:
    enum AdressWidth
    {
        AW_3BYTES,
        AW_4BYTES,
        AW_5BYTES,
    };
    void writeReg(unsigned char reg, unsigned char size, unsigned char *data);
    void readReg(unsigned char reg, unsigned char size, unsigned char *data);


    // CONFIG register
    void setConfig(unsigned char maskRXDataReady,
            unsigned char maskTXDataSent,
            unsigned char maskMaxRetriesReached,
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

    // EN_RXARRD
    void enablePipe(unsigned char pipe, unsigned char value);

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
    void setRXAddress(unsigned char channel, unsigned char* address);

    // TX_ADDR
    void setTXAddress(unsigned char* address);

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
    unsigned char m_status;

    unsigned char m_config;
    unsigned char m_RFChannel;
};

extern RadioManager radio;


#endif /* LAZERTAG_RIFLE_INCLUDE_RADIO_HPP_ */


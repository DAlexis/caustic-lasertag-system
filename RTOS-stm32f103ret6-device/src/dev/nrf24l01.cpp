/*
 * nrf24l01.cpp
 *
 *  Created on: 25 июня 2015 г.
 *      Author: alexey
 */

#include "dev/nrf24l01.hpp"
#include "core/logging.hpp"
#include "core/os-wrappers.hpp"
#include "core/string-utils.hpp"
#include "hal/system-clock.hpp"
#include <stdio.h>
#include <string.h>
/*
 * NRF24l01 Registers
 */

#define NRF_REG_CONFIG          0x00
#define NRF_REG_EN_AA           0x01
#define NRF_REG_EN_RXADDR       0x02
#define NRF_REG_SETUP_AW        0x03
#define NRF_REG_SETUP_RETR      0x04
#define NRF_REG_RF_CH           0x05
#define NRF_REG_RF_SETUP        0x06
#define NRF_REG_STATUS          0x07
#define NRF_REG_OBSERVE_TX      0x08
#define NRF_REG_CD              0x09
#define NRF_REG_RX_ADDR_P0      0x0A
#define NRF_REG_RX_ADDR_P1      0x0B
#define NRF_REG_RX_ADDR_P2      0x0C
#define NRF_REG_RX_ADDR_P3      0x0D
#define NRF_REG_RX_ADDR_P4      0x0E
#define NRF_REG_RX_ADDR_P5      0x0F
#define NRF_REG_TX_ADDR         0x10
#define NRF_REG_RX_PW_P0        0x11
#define NRF_REG_RX_PW_P1        0x12
#define NRF_REG_RX_PW_P2        0x13
#define NRF_REG_RX_PW_P3        0x14
#define NRF_REG_RX_PW_P4        0x15
#define NRF_REG_RX_PW_P5        0x16
#define NRF_REG_FIFO_STATUS     0x17
// N/A                          0x18
// N/A                          0x19
// N/A                          0x1A
// N/A                          0x1B
#define NRF_REG_DYNPD           0x1C
#define NRF_REG_FEATURE         0x1D


// Operations
#define NRF_NOP                 0xFF

/*
 * NRF2401 Register Fields
 */

// CONFIG
#define NRF_REGF_PRIM_RX            0
#define NRF_REGF_PWR_UP             1
#define NRF_REGF_CRCO               2
#define NRF_REGF_EN_CRC             3
#define NRF_REGF_MASK_MAX_RT        4
#define NRF_REGF_MASK_TX_DS         5
#define NRF_REGF_MASK_RX_DR         6

// EN_AA
#define NRF_REGF_ENAA_P0            0
#define NRF_REGF_ENAA_P1            1
#define NRF_REGF_ENAA_P2            2
#define NRF_REGF_ENAA_P3            3
#define NRF_REGF_ENAA_P4            4
#define NRF_REGF_ENAA_P5            5

// EN_RXADDR
#define NRF_REGF_ERX_P0             0
#define NRF_REGF_ERX_P1             1
#define NRF_REGF_ERX_P2             2
#define NRF_REGF_ERX_P3             3
#define NRF_REGF_ERX_P4             4
#define NRF_REGF_ERX_P5             5

// SETUP_AW
#define NRF_REGF_AW                 0

// SETUP_RETR
#define NRF_REGF_ARC                0
#define NRF_REGF_ARD                4

// RF_CH
#define NRF_REGF_RF_CH              0

// RF_SETUP
#define NRF_REGF_LNA_HCURR          0
#define NRF_REGF_RF_PWR             1
#define NRF_REGF_RF_DR              3
#define NRF_REGF_PLL_LOCK           4

// STATUS
#define NRF_REGF_TX_FULL            0
#define NRF_REGF_MAX_RT             4
#define NRF_REGF_TX_DS              5
#define NRF_REGF_RX_DR              6
#define NRF_REGF_RX_P_NO_MASK       0b00001110

// OBSERVE_TX
#define NRF_REGF_ARC_CNT_MASK            0b00001111
#define NRF_REGF_PLOS_CNT           4

// CD
#define NRF_REGF_CD                 0

// ADDR
#define NRF_REGF_ADDR_A             0
#define NRF_REGF_ADDR_B             1
#define NRF_REGF_ADDR_C             2
#define NRF_REGF_ADDR_D             3
#define NRF_REGF_ADDR_E             4

// RX_PW
#define NRF_REGF_PW                 0

// FIFO_STATUS
#define NRF_REGF_FIFO_RX_EMPTY      0
#define NRF_REGF_FIFO_RX_FULL       1
#define NRF_REGF_FIFO_TX_EMPTY      4
#define NRF_REGF_FIFO_TX_FULL       5
#define NRF_REGF_FIFO_TX_REUSE      6

// DYNPD
#define NRF_REGF_DPL_P0         0
#define NRF_REGF_DPL_P1         1
#define NRF_REGF_DPL_P2         2
#define NRF_REGF_DPL_P3         3
#define NRF_REGF_DPL_P4         4
#define NRF_REGF_DPL_P5         5

// FEATURE
#define NRF_REGF_EN_DYN_ACK     0
#define NRF_REGF_EN_ACK_PAY     1
#define NRF_REGF_EN_DPL         2

// Instructions
#define R_REGISTER(reg)         (0b00011111 & reg)
#define W_REGISTER(reg)         (0b00100000 | reg)
#define R_RX_PAYLOAD            0b01100001
#define W_TX_PAYLOAD            0b10100000
#define FLUSH_TX                0b11100001
#define FLUSH_RX                0b11100010
#define REUSE_TX_PL             0b11100011



NRF24L01Manager::NRF24L01Manager()
{
}

void NRF24L01Manager::init(IIOPin* chipEnablePin, IIOPin* chipSelectPin, IIOPin* IRQPin, uint8_t SPIIndex, bool useInterrupts)
{
    debug << "Radio module initialization...\n";
    m_chipEnablePin = chipEnablePin;
    m_chipSelectPin = chipSelectPin;
    m_IRQPin = IRQPin;
    m_spi = SPIs->getSPI(SPIIndex);
    m_spi->init(ISPIManager::BaudRatePrescaler8, chipSelectPin);

    //////////////////////
    // Chip enable line init
    m_chipEnablePin->switchToOutput();
    chipEnableOff();

    //////////////////////
    // Chip select SPI line init
    //m_chipSelectPin->switchToOutput();
    chipDeselect();

    // IRQ line input
    m_IRQPin->switchToInput();

    //////////////////////
    // Configuring nrf24l01
    // Global settings
    info << "setAdressWidth(AW_5BYTES);";
    setAdressWidth(AW_5BYTES);
    info << "etRFChannel(m_RFChannel);";
    setRFChannel(m_RFChannel);
    //setupRetransmission(0, 0);

    // Reading addresses to local variables
    info << "readRXAdresses";
    readRXAdresses();
    info << "readTXAdress";
    readTXAdress();

    // Setting up pipe 0
    info << "setAutoACK";
    setAutoACK(0, DISABLE_OPTION);
    info << "enablePipe";
    enablePipe(0, ENABLE_OPTION);
    info << "setRXPayloadLength";
    setRXPayloadLength(0, payloadSize);

    info << "setRXPayloadLength";
    setRFSettings(BR_1MBIT, PW_0DB, LNA_ENABLE);
    info << "setupRetransmission";
    setupRetransmission(2, 15);
    info << "switchToRX";
    switchToRX();

    m_useInterrupts = useInterrupts;
    if (m_useInterrupts)
	{
    	IRQPin->setExtiCallback(std::bind(&NRF24L01Manager::extiHandler, this, std::placeholders::_1));
		IRQPin->enableExti(true);
		if (false == m_IRQPin->state())
			m_needInterrogation = true;
		else
			m_needInterrogation = false;
	}

    if (m_TXAdress[0] != 0xe7)
	{
		error << "Radio module seems to be not connected! Details:\n";
		printStatus();
	}
}

void NRF24L01Manager::setDataReceiveCallback(DataReceiveCallback callback)
{
    m_RXcallback = callback;
}

void NRF24L01Manager::setTXMaxRetriesCallback(TXMaxRetriesCallback callback)
{
    m_TXMaxRTcallback = callback;
}

void NRF24L01Manager::setTXDoneCallback(TXDoneCallback callback)
{
    m_TXDoneCallback = callback;
}

void NRF24L01Manager::switchToTX()
{
    chipEnableOff();
    setConfig(IM_MASK_MAX_RETRIES, CRC_ENABLE, CRC2BYTES, POWER_ON, MODE_TRANSMITTER);
}

void NRF24L01Manager::switchToRX()
{
    setConfig(IM_MASK_MAX_RETRIES, CRC_ENABLE, CRC2BYTES, POWER_ON, MODE_RECEIVER);
    chipEnableOn();
}


void NRF24L01Manager::chipEnableOn()
{
	m_chipEnablePin->set();
}

void NRF24L01Manager::chipEnableOff()
{
    //for (volatile int i=0; i<300; i++) { }
	m_chipEnablePin->reset();
}

void NRF24L01Manager::chipSelect()
{
	m_chipSelectPin->reset();
}

void NRF24L01Manager::chipDeselect()
{
	m_chipSelectPin->set();
}


void NRF24L01Manager::CEImpulse()
{
	taskENTER_CRITICAL();
	{
		chipEnableOn();
		systemClock->wait_us(15);
		chipEnableOff();
	}
	taskEXIT_CRITICAL();
}

void NRF24L01Manager::writeReg(unsigned char reg, unsigned char size, unsigned char *data)
{
    chipSelect();
	//HAL_SPI_TransmitReceive
    m_status = m_spi->TransmitReceive(W_REGISTER(reg));
    if (size != 0) m_spi->Transmit(data, size);
    chipDeselect();
}

void NRF24L01Manager::readReg(unsigned char reg, unsigned char size, unsigned char *data)
{
    chipSelect();
	m_status = m_spi->TransmitReceive(R_REGISTER(reg));
	if (size != 0) m_spi->Receive(data, size);
    chipDeselect();
}

void NRF24L01Manager::updateStatus()
{
    chipSelect();
    m_status = m_spi->TransmitReceive(NRF_NOP);
    chipDeselect();
}

/////////////////////
// STATUS
inline bool NRF24L01Manager::isRXDataReady()
{
    return m_status & (1 << NRF_REGF_RX_DR);
}

inline bool NRF24L01Manager::isTXDataSent()
{
    return m_status & (1 << NRF_REGF_TX_DS);
}

inline bool NRF24L01Manager::isMaxRetriesReached()
{
    return m_status & (1 << NRF_REGF_MAX_RT);
}

inline int NRF24L01Manager::getPipeNumberAvaliableForRXFIFO()
{
    return (m_status & NRF_REGF_RX_P_NO_MASK) >> 1;
}

inline bool NRF24L01Manager::isTXFIFOFull()
{
    return m_status & (1 << NRF_REGF_TX_FULL);
}

void NRF24L01Manager::resetRXDataReady()
{
    chipSelect();
    m_spi->Transmit(W_REGISTER(NRF_REG_STATUS));
    m_spi->Transmit(1 << NRF_REGF_RX_DR);
    chipDeselect();
    updateStatus();
}

void NRF24L01Manager::resetTXDataSent()
{
    chipSelect();
    m_spi->Transmit(W_REGISTER(NRF_REG_STATUS));
    m_spi->Transmit(1 << NRF_REGF_TX_DS);
    chipDeselect();
    updateStatus();
}

void NRF24L01Manager::resetMaxRetriesReached()
{
    chipSelect();
    m_spi->Transmit(W_REGISTER(NRF_REG_STATUS));
    m_spi->Transmit(1 << NRF_REGF_MAX_RT);
    chipDeselect();
    updateStatus();
}


/////////////////////
// CONFIG
void NRF24L01Manager::setConfig(unsigned char interruptionsMask,
            unsigned char enableCRC,
            unsigned char CRC2bytes,
            unsigned char powerUP,
            unsigned char isRecieving)
{
    m_config =
            (interruptionsMask << NRF_REGF_MASK_MAX_RT)
            | (enableCRC << NRF_REGF_EN_CRC)
            | (CRC2bytes << NRF_REGF_CRCO)
            | (powerUP << NRF_REGF_PWR_UP)
            | (isRecieving << NRF_REGF_PRIM_RX);
    //printf("Config: %u\n", m_config);
    writeReg(NRF_REG_CONFIG, 1, &m_config);
}

/////////////////////
// CD
bool NRF24L01Manager::isCarrierDetected()
{
    unsigned char result=0;
    readReg(NRF_REG_CD, 1, &result);
    return result & (1 << NRF_REGF_CD);
}


/////////////////////
// EN_AA
void NRF24L01Manager::setAutoACK(unsigned char channel, bool value)
{
    unsigned char regValue=0;
    readReg(NRF_REG_EN_AA, 1, &regValue);
    if (value)
    {
        regValue |= (1 << channel);
    } else {
        regValue &= ~(1 << channel);
    }
    //printf("to EN_AA: %u\n", regValue);
    writeReg(NRF_REG_EN_AA, 1, &regValue);
}

/////////////////////
// EN_RXADDR
void NRF24L01Manager::enablePipe(unsigned char pipe, unsigned char value)
{
    unsigned char regValue=0;
    readReg(NRF_REG_EN_RXADDR, 1, &regValue);
    if (value)
        regValue |= (1 << pipe);
    else
        regValue &= ~(1 << pipe);
    //printf("to NRF_REG_EN_RXADDR: %u\n", regValue);
    writeReg(NRF_REG_EN_RXADDR, 1, &regValue);
}

/////////////////////
// SETUP_AW
void NRF24L01Manager::setAdressWidth(AdressWidth width)
{
    unsigned char regValue=0;
    switch(width)
    {
    case AW_3BYTES: regValue |= 0b00000001; break;
    case AW_4BYTES: regValue |= 0b00000010; break;
    case AW_5BYTES: regValue |= 0b00000011; break;
    }
    writeReg(NRF_REG_SETUP_AW, 1, &regValue);
}

/////////////////////
// SETUP_RETR
void NRF24L01Manager::setupRetransmission(unsigned char delay, unsigned char count)
{
    unsigned char regValue = (delay << NRF_REGF_ARD) | (count << NRF_REGF_ARC);
    writeReg(NRF_REG_SETUP_RETR, 1, &regValue);
}

/////////////////////
// RF_CH
void NRF24L01Manager::setRFChannel(unsigned char number)
{
    m_RFChannel = number & 0b01111111;
    writeReg(NRF_REG_RF_CH, 1, &m_RFChannel);
}

void NRF24L01Manager::clearLostPackagesCount()
{
    writeReg(NRF_REG_RF_CH, 1, &m_RFChannel);
}

/////////////////////
// RF_SETUP
void NRF24L01Manager::setRFSettings(unsigned char use2MBits, unsigned char power, unsigned char LNAGain)
{
    unsigned char regValue = (use2MBits << NRF_REGF_RF_DR) | (power << NRF_REGF_RF_PWR) | (LNAGain << NRF_REGF_LNA_HCURR);
    writeReg(NRF_REG_RF_SETUP, 1, &regValue);
}

/////////////////////
// OBSERVE_TX
unsigned char NRF24L01Manager::getLostPackagesCount()
{
    unsigned char regValue=0;
    readReg(NRF_REG_OBSERVE_TX, 1, &regValue);
    return regValue >> NRF_REGF_PLOS_CNT;
}

unsigned char NRF24L01Manager::getResentPackagesCount()
{
    unsigned char regValue=0;
    readReg(NRF_REG_OBSERVE_TX, 1, &regValue);
    return regValue & NRF_REGF_ARC_CNT_MASK;
}

/////////////////////
// RX_ADDR_Pn
void NRF24L01Manager::setRXAddress(unsigned char channel, unsigned char* address)
{

    switch(channel)
    {
    default:
    case 0:
        memcpy(m_RXAdressP0, address, RADIO_ADDRESS_SIZE*sizeof(unsigned char));
        writeReg(NRF_REG_RX_ADDR_P0, RADIO_ADDRESS_SIZE, address);
        break;
    case 1:
        memcpy(m_RXAdressP1, address, RADIO_ADDRESS_SIZE*sizeof(unsigned char));
        writeReg(NRF_REG_RX_ADDR_P1, RADIO_ADDRESS_SIZE, address);
        break;
    case 2:
        m_RXAdressP2 = *address;
        writeReg(NRF_REG_RX_ADDR_P2, 1, address);
        break;
    case 3:
        m_RXAdressP3 = *address;
        writeReg(NRF_REG_RX_ADDR_P3, 1, address);
        break;
    case 4:
        m_RXAdressP4 = *address;
        writeReg(NRF_REG_RX_ADDR_P4, 1, address);
        break;
    case 5:
        m_RXAdressP5 = *address;
        writeReg(NRF_REG_RX_ADDR_P5, 1, address);
        break;
    }
}

void NRF24L01Manager::readRXAdresses()
{
    readReg(NRF_REG_RX_ADDR_P0, 5, m_RXAdressP0);
    readReg(NRF_REG_RX_ADDR_P1, 5, m_RXAdressP1);
    readReg(NRF_REG_RX_ADDR_P2, 1, &m_RXAdressP2);
    readReg(NRF_REG_RX_ADDR_P3, 1, &m_RXAdressP3);
    readReg(NRF_REG_RX_ADDR_P4, 1, &m_RXAdressP4);
    readReg(NRF_REG_RX_ADDR_P5, 1, &m_RXAdressP5);
}

/////////////////////
// TX_ADDR
void NRF24L01Manager::setTXAddress(unsigned char* address)
{
    memcpy(m_TXAdress, address, RADIO_ADDRESS_SIZE*sizeof(unsigned char));
    writeReg(NRF_REG_TX_ADDR, RADIO_ADDRESS_SIZE, m_TXAdress);
}

void NRF24L01Manager::readTXAdress()
{
    readReg(NRF_REG_TX_ADDR, RADIO_ADDRESS_SIZE, m_TXAdress);
}

/////////////////////
// RX_PW_Pn
void NRF24L01Manager::setRXPayloadLength(unsigned char channel, unsigned char payloadLength)
{
    payloadLength &= 0b00111111;
    //printf("Payload len: %u\n", payloadLength);
    switch(channel)
    {
    default:
    case 0: writeReg(NRF_REG_RX_PW_P0, 1, &payloadLength); break;
    case 1: writeReg(NRF_REG_RX_PW_P1, 1, &payloadLength); break;
    case 2: writeReg(NRF_REG_RX_PW_P2, 1, &payloadLength); break;
    case 3: writeReg(NRF_REG_RX_PW_P3, 1, &payloadLength); break;
    case 4: writeReg(NRF_REG_RX_PW_P4, 1, &payloadLength); break;
    case 5: writeReg(NRF_REG_RX_PW_P5, 1, &payloadLength); break;
    }
}

/////////////////////
// FIFO_STATUS
bool NRF24L01Manager::isReuseEnabled()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_TX_REUSE));
}

bool NRF24L01Manager::isTXFull()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_TX_FULL));
}

bool NRF24L01Manager::isTXEmpty()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_TX_EMPTY));
}

bool NRF24L01Manager::isRXFull()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_RX_FULL));
}

bool NRF24L01Manager::isRXEmpty()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_RX_EMPTY));
}

/////////////////////
// Send and receive
void NRF24L01Manager::sendData(unsigned char size, unsigned char* data)
{
	if (m_debug)
		printHex(data, size);
    switchToTX();
    chipSelect();
    m_status = m_spi->TransmitReceive(W_TX_PAYLOAD);
    m_spi->Transmit(data, size);
    chipDeselect();
    CEImpulse();
}

void NRF24L01Manager::receiveData(unsigned char size, unsigned char* data)
{
    chipEnableOff();
    chipSelect();
    m_status = m_spi->TransmitReceive(R_RX_PAYLOAD);
    m_spi->Receive(data, size);
    chipDeselect();
}

void NRF24L01Manager::flushTX()
{
    chipSelect();
    m_status = m_spi->TransmitReceive(FLUSH_TX);
    chipDeselect();
}

void NRF24L01Manager::flushRX()
{
    chipSelect();
    m_status = m_spi->TransmitReceive(FLUSH_RX);
    chipDeselect();
}

void NRF24L01Manager::printStatus()
{
    updateStatus();
    readRXAdresses();
	readTXAdress();
    printf("status: %x\n", m_status);
    if (isRXDataReady()) printf("RX Data ready\n");
    if (isTXDataSent()) printf("TX Data sent\n");
    if (isMaxRetriesReached()) printf("Max retries reached\n");
    printf ("Pipe avaliable for RX FIFO: %d\n", getPipeNumberAvaliableForRXFIFO());

    if (isReuseEnabled()) printf ("Reuse enabled\n");
    if (isTXFull()) printf("TX full\n");
    if (isTXEmpty()) printf("TX empty\n");
    if (isRXFull()) printf("RX full\n");
    if (isRXEmpty()) printf ("RX empty\n");
    printf("Lost: %u, resent: %u\n", getLostPackagesCount(), getResentPackagesCount());

    printf("Adresses:\n");
    printf("   TX: %x %x %x %x %x\n", m_TXAdress[0], m_TXAdress[1], m_TXAdress[2], m_TXAdress[3], m_TXAdress[4]);
    printf("RX P0: %x %x %x %x %x\n", m_RXAdressP0[0], m_RXAdressP0[1], m_RXAdressP0[2], m_RXAdressP0[3], m_RXAdressP0[4]);
    printf("RX P1: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP1[4]);
    printf("RX P2: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP2);
    printf("RX P3: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP3);
    printf("RX P4: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP4);
    printf("RX P5: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP5);
}

void NRF24L01Manager::resetAllIRQ()
{
    resetRXDataReady();
    resetTXDataSent();
    resetMaxRetriesReached();
}


////////////////////
// Interrupts handling

void NRF24L01Manager::interrogate()
{
	//info << "ri";
	
	if (m_useInterrupts)
	{
		if (!m_needInterrogation)
			return;
		m_needInterrogation = false;
	} else {
		if (true == m_IRQPin->state())
			return;
	}

    updateStatus();
    if (isRXDataReady())
    {
    	//info << "RX data ready";
        unsigned char pipe = getPipeNumberAvaliableForRXFIFO();
        unsigned char data[payloadSize];
        while (!isRXEmpty())
        {
            receiveData(payloadSize, data);
            if (m_RXcallback == nullptr) {
                printf("Warning: Callback is not set! RX data from pipe %d: \n", pipe);
                printf("%x %x %x %x %x\n", data[0], data[1], data[2], data[3], data[4]);
            } else {
            	//info << "RX callback called";
                m_RXcallback(pipe, data);
            }
        }
        chipEnableOn();
        //info << "resetRXDataReady";
        resetRXDataReady();
    }
    if (isTXDataSent())
    {
        // Returning to default state: receiver
    	switchToRX();
        resetTXDataSent();
        if (m_TXDoneCallback == nullptr) {
            printf("TX done; no cb\n");
        } else
            m_TXDoneCallback();
    }
    if (isMaxRetriesReached())
    {
        if (m_TXMaxRTcallback == nullptr) {
            printf("Max RT; no cb\n");
        } else {
            m_TXMaxRTcallback();
        }
        // Clearing PLOS_CNT
        setRFChannel(RADIO_CHANNEL);

        resetMaxRetriesReached();
    }
    //resetAllIRQ();
}

void NRF24L01Manager::extiHandler(bool state)
{
	//info << "NRF24l01 IRQ! state: " << state;
	if (true == state)
		return;

	updateStatus();
	/// @todo Create function with code below
	if (isTXDataSent())
	{
		// Returning to default state: receiver
		switchToRX();
		resetTXDataSent();
		if (m_TXDoneCallback == nullptr) {
			printf("TX done; no cb\n");
		} else
			m_TXDoneCallback();
	}
	m_needInterrogation = true;
}

void NRF24L01Manager::enableDebug(bool debug)
{
	m_debug = debug;
}

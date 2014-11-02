/*
 * radio.cpp
 *
 *  Created on: 19 окт. 2014 г.
 *      Author: alexey
 */


#include "radio.hpp"
#include "utils.hpp"
#include <stdio.h>
#include <string.h>


/*
 * NRF2041 Registers
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


RadioManager radio;

RadioManager::RadioManager() :
    m_status(0),
    m_config(0),
    m_RFChannel(1)
{
}

void RadioManager::init()
{
    printf("Radio module initialization...\n");
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    SPI2Manager.init(SPI_BaudRatePrescaler_8);

    //////////////////////
    // Chip enable line init
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    chipEnableOff();

    //////////////////////
    // Chip select SPI line init
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    chipDeselect();

    //////////////////////
    // Configuring nrf24l01
    readRXAdresses();
    readTXAdress();
    setAutoACK(0, DISABLE_OPTION);
    enablePipe(0, ENABLE_OPTION);
    setAdressWidth(AW_5BYTES);
    setRFChannel(m_RFChannel);
    setRXPayloadLength(0, PAYLOAD_SIZE);
    //setRXPayloadLength(1, PAYLOAD_SIZE);
    /*setAutoACK(1, 0);
    setAutoACK(2, 0);
    setAutoACK(3, 0);
    setAutoACK(4, 0);
    setAutoACK(5, 0);
    setupRetransmission(0, 0);*/
    setRFSettings(BR_1MBIT, PW_0DB, LNA_ENABLE);
    setRXPayloadLength(0, 5);
    setupRetransmission(2, 15);
    switchToRX();
    initInterrupts();
}

void RadioManager::switchToTX()
{
    chipEnableOff();
    setConfig(IM_MASK_MAX_RETRIES, CRC_ENABLE, CRC2BYTES, POWER_ON, MODE_TRANSMITTER);
}

void RadioManager::switchToRX()
{
    setConfig(IM_MASK_MAX_RETRIES, CRC_ENABLE, CRC2BYTES, POWER_ON, MODE_RECEIVER);
    chipEnableOn();
}


void RadioManager::chipEnableOn()
{
    GPIO_SetBits(GPIOB, GPIO_Pin_11);
}

void RadioManager::chipEnableOff()
{
    //for (volatile int i=0; i<300; i++) { }
    GPIO_ResetBits(GPIOB, GPIO_Pin_11);
}

void RadioManager::chipSelect()
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void RadioManager::chipDeselect()
{
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void RadioManager::CEImpulse()
{
    chipEnableOn();
    systemTimer.delay(20);
    chipEnableOff();
}

void RadioManager::writeReg(unsigned char reg, unsigned char size, unsigned char *data)
{
    chipSelect();
    m_status = SPI2Manager.send_single(W_REGISTER(reg));
    if (size != 0) SPI2Manager.send(data, size);
    chipDeselect();
}

void RadioManager::readReg(unsigned char reg, unsigned char size, unsigned char *data)
{
    chipSelect();
    m_status = SPI2Manager.send_single(R_REGISTER(reg));
    if (size != 0) SPI2Manager.receive(data, size);
    chipDeselect();
}

void RadioManager::updateStatus()
{
    chipSelect();
    m_status = SPI2Manager.send_single(NRF_NOP);
    chipDeselect();
}

/////////////////////
// STATUS
inline bool RadioManager::isRXDataReady()
{
    return m_status & (1 << NRF_REGF_RX_DR);
}

inline bool RadioManager::isTXDataSent()
{
    return m_status & (1 << NRF_REGF_TX_DS);
}

inline bool RadioManager::isMaxRetriesReached()
{
    return m_status & (1 << NRF_REGF_MAX_RT);
}

inline int RadioManager::getPipeNumberAvaliableForRXFIFO()
{
    return (m_status & NRF_REGF_RX_P_NO_MASK) >> 1;
}

inline bool RadioManager::isTXFIFOFull()
{
    return m_status & (1 << NRF_REGF_TX_FULL);
}

void RadioManager::resetRXDataReady()
{
    chipSelect();
    SPI2Manager.send_single(W_REGISTER(NRF_REG_STATUS));
    SPI2Manager.send_single(1 << NRF_REGF_RX_DR);
    chipDeselect();
    updateStatus();
}

void RadioManager::resetTXDataSent()
{
    chipSelect();
    SPI2Manager.send_single(W_REGISTER(NRF_REG_STATUS));
    SPI2Manager.send_single(1 << NRF_REGF_TX_DS);
    chipDeselect();
    updateStatus();
}

void RadioManager::resetMaxRetriesReached()
{
    chipSelect();
    SPI2Manager.send_single(W_REGISTER(NRF_REG_STATUS));
    SPI2Manager.send_single(1 << NRF_REGF_MAX_RT);
    chipDeselect();
    updateStatus();
}


/////////////////////
// CONFIG
void RadioManager::setConfig(unsigned char interruptionsMask,
            unsigned char enableCRC,
            unsigned char CRC2bytes,
            unsigned char powerUP,
            unsigned char isRecieving)
{
    m_config = /*(maskRXDataReady << NRF_REGF_MASK_RX_DR)
            | (maskTXDataSent << NRF_REGF_MASK_TX_DS) |*/
            (interruptionsMask << NRF_REGF_MASK_MAX_RT)
            | (enableCRC << NRF_REGF_EN_CRC)
            | (CRC2bytes << NRF_REGF_CRCO)
            | (powerUP << NRF_REGF_PWR_UP)
            | (isRecieving << NRF_REGF_PRIM_RX);
    printf("Config: %u\n", m_config);
    writeReg(NRF_REG_CONFIG, 1, &m_config);
}

void RadioManager::switchToReceiver()
{
    m_config |= (1 << NRF_REGF_PRIM_RX);
    writeReg(NRF_REG_CONFIG, 1, &m_config);
}

void RadioManager::switchToTranmitter()
{
    m_config &= ~(1 << NRF_REGF_PRIM_RX);
    writeReg(NRF_REG_CONFIG, 1, &m_config);
}


/////////////////////
// CD

bool RadioManager::isCarrierDetected()
{
    unsigned char result=0;
    readReg(NRF_REG_CD, 1, &result);
    return result & (1 << NRF_REGF_CD);
}


/////////////////////
// EN_AA
void RadioManager::setAutoACK(unsigned char channel, bool value)
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
void RadioManager::enablePipe(unsigned char pipe, unsigned char value)
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
void RadioManager::setAdressWidth(AdressWidth width)
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
void RadioManager::setupRetransmission(unsigned char delay, unsigned char count)
{
    unsigned char regValue = (delay << NRF_REGF_ARD) | (count << NRF_REGF_ARC);
    writeReg(NRF_REG_SETUP_RETR, 1, &regValue);
}

/////////////////////
// RF_CH
void RadioManager::setRFChannel(unsigned char number)
{
    m_RFChannel = number & 0b01111111;
    writeReg(NRF_REG_RF_CH, 1, &m_RFChannel);
}

void RadioManager::clearLostPackagesCount()
{
    writeReg(NRF_REG_RF_CH, 1, &m_RFChannel);
}

/////////////////////
// RF_SETUP
void RadioManager::setRFSettings(unsigned char use2MBits, unsigned char power, unsigned char LNAGain)
{
    unsigned char regValue = (use2MBits << NRF_REGF_RF_DR) | (power << NRF_REGF_RF_PWR) | (LNAGain << NRF_REGF_LNA_HCURR);
    writeReg(NRF_REG_RF_SETUP, 1, &regValue);
}

/////////////////////
// OBSERVE_TX
unsigned char RadioManager::getLostPackagesCount()
{
    unsigned char regValue=0;
    readReg(NRF_REG_OBSERVE_TX, 1, &regValue);
    return regValue >> NRF_REGF_PLOS_CNT;
}

unsigned char RadioManager::getResentPackagesCount()
{
    unsigned char regValue=0;
    readReg(NRF_REG_OBSERVE_TX, 1, &regValue);
    return regValue & NRF_REGF_ARC_CNT_MASK;
}

/////////////////////
// RX_ADDR_Pn
void RadioManager::setRXAddress(unsigned char channel, unsigned char* address)
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

void RadioManager::readRXAdresses()
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
void RadioManager::setTXAddress(unsigned char* address)
{
    memcpy(m_TXAdress, address, RADIO_ADDRESS_SIZE*sizeof(unsigned char));
    writeReg(NRF_REG_TX_ADDR, RADIO_ADDRESS_SIZE, m_TXAdress);
}

void RadioManager::readTXAdress()
{
    readReg(NRF_REG_TX_ADDR, RADIO_ADDRESS_SIZE, m_TXAdress);
}

/////////////////////
// RX_PW_Pn
void RadioManager::setRXPayloadLength(unsigned char channel, unsigned char payloadLength)
{
    payloadLength &= 0b00111111;
    printf("Payload len: %u\n", payloadLength);
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
bool RadioManager::isReuseEnabled()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_TX_REUSE));
}

bool RadioManager::isTXFull()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_TX_FULL));
}

bool RadioManager::isTXEmpty()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_TX_EMPTY));
}

bool RadioManager::isRXFull()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_RX_FULL));
}

bool RadioManager::isRXEmpty()
{
    unsigned char regValue;
    readReg(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_RX_EMPTY));
}

/////////////////////
// Send and receive
void RadioManager::sendData(unsigned char size, unsigned char* data)
{
    chipEnableOff();
    chipSelect();
    m_status = SPI2Manager.send_single(W_TX_PAYLOAD);
    SPI2Manager.send(data, size);
    chipDeselect();
    CEImpulse();

}

void RadioManager::receiveData(unsigned char size, unsigned char* data)
{
    chipEnableOff();
    chipSelect();
    m_status = SPI2Manager.send_single(R_RX_PAYLOAD);
    SPI2Manager.receive(data, size);
    chipDeselect();
}

void RadioManager::flushTX()
{
    chipSelect();
    m_status = SPI2Manager.send_single(FLUSH_TX);
    chipDeselect();
}

void RadioManager::flushRX()
{
    chipSelect();
    m_status = SPI2Manager.send_single(FLUSH_RX);
    chipDeselect();
}

void RadioManager::printStatus()
{
    updateStatus();
    printf("Value: %u\n", m_status);
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

void RadioManager::resetAllIRQ()
{
    resetRXDataReady();
    resetTXDataSent();
    resetMaxRetriesReached();
}


////////////////////
// Interrupts handling
void RadioManager::initInterrupts()
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);

    // Configure EXTI10 line
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Enable and set EXTI10 Interrupt to the lowest priority
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void RadioManager::interruptHandler()
{
    updateStatus();
    if (isRXDataReady())
    {
        int pipe = getPipeNumberAvaliableForRXFIFO();
        printf("RX data from pipe %d: ", pipe);
        unsigned char data[PAYLOAD_SIZE];
        receiveData(PAYLOAD_SIZE, data);
        printf("%x %x %x %x %x\n", data[0], data[1], data[2], data[3], data[4]);
        chipEnableOn();
        resetRXDataReady();
    }
    if (isTXDataSent())
    {
        //switchToReceiver();
        resetTXDataSent();
    }
    if (isMaxRetriesReached())
    {
        resetMaxRetriesReached();
    }
    //resetAllIRQ();
}

extern "C" {
    void EXTI15_10_IRQHandler()
    {
        if(EXTI_GetITStatus(EXTI_Line10) != RESET)
        {
            radio.interruptHandler();
            EXTI_ClearITPendingBit(EXTI_Line10);
        }
    }
}

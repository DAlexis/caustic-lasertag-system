/*
 * radio.cpp
 *
 *  Created on: 19 окт. 2014 г.
 *      Author: alexey
 */


#include "radio.hpp"
#include <stdio.h>

/**
 * Maximum number of bytes needed to store register information (largest
 * block of data to be read from nNRF is the address register with 5 bytes).
 */
#define NRF_MAX_REG_BUF         5

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
#define NRF_REG_RX_ADDR_P5      0x1F
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
#define NRF_REGF_ARD                1

// RF_CH
#define NRF_REGF_RF_CH              0

// RF_SETUP
#define NRF_REGF_LNA_HCURR          0
#define NRF_REGF_RF_PWR             1
#define NRF_REGF_RF_DR              2
#define NRF_REGF_PLL_LOCK           3

// STATUS
#define NRF_REGF_TX_FULL            0
#define NRF_REGF_RX_P_NO            1
#define NRF_REGF_MAX_RT             2
#define NRF_REGF_TX_DS              3
#define NRF_REGF_RX_DR              4

// OBSERVE_TX
#define NRF_REGF_ARC_CNT            0
#define NRF_REGF_PLOS_CNT           1

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

#define R_REGISTER(reg)         (0b00011111 & reg)
#define W_REGISTER(reg)         (0b00100000 | reg)

RadioManager radio;

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
    //chipEnableOn();
    chipEnableOff();

    //////////////////////
    // Chip select SPI line init
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    chipDeselect();

}

void RadioManager::chipEnableOn()
{
    GPIO_SetBits(GPIOB, GPIO_Pin_11);
}

void RadioManager::chipEnableOff()
{
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

void RadioManager::readTXAdress()
{
    chipSelect();
    unsigned char status = SPI2Manager.send_single(NRF_REG_TX_ADDR);

    unsigned char result[5];
    SPI2Manager.receive(result, 5);
    chipDeselect();
    printf("status: %d, result: %d %d %d %d %d\n", (int) status,
            (int) result[0], (int) result[1], (int) result[2], (int) result[3], (int) result[4]);
}

void RadioManager::writeTXAdress()
{
    chipSelect();
    unsigned char status = SPI2Manager.send_single(W_REGISTER(NRF_REG_TX_ADDR));

    unsigned char adress[5]={0xFF, 0x00, 0xFF, 0x00, 0xFF};
    SPI2Manager.send(adress, 5);
    chipDeselect();
    printf("status: %d\n", (int) status);
}

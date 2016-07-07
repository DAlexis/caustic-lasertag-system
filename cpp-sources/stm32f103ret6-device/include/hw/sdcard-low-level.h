/*
 * sdcard-low-level.h
 *
 *  Created on: 08 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_SDCARD_LOW_LEVEL_H_
#define LAZERTAG_RIFLE_INCLUDE_HW_SDCARD_LOW_LEVEL_H_

#include <stdint.h>

#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40018080)

#define SDIO_TRANSFER_CLK_DIV            ((uint8_t)0x08)
#define SDIO_INIT_CLK_DIV                ((uint8_t)0xB2)

void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void);
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
uint32_t SD_DMAEndOfTransferStatus(void);



#endif /* LAZERTAG_RIFLE_INCLUDE_HW_SDCARD_LOW_LEVEL_H_ */

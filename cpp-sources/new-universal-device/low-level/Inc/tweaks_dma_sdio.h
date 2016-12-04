/*
 * tweaks_dma_sdio.h
 *
 *  Created on: 2 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef NEW_UNIVERSAL_DEVICE_INC_TWEAKS_DMA_SDIO_H_
#define NEW_UNIVERSAL_DEVICE_INC_TWEAKS_DMA_SDIO_H_

typedef enum {
	TWEAK_DMA_SDIO_DIR_TO_DEVICE = 0,
	TWEAK_DMA_SDIO_DIR_TO_MEMORY
} TWEAK_DMA_SDIO_Direction;

void TWEAK_Set_DMA_SDIO_Direction(TWEAK_DMA_SDIO_Direction direction);


#endif /* NEW_UNIVERSAL_DEVICE_INC_TWEAKS_DMA_SDIO_H_ */

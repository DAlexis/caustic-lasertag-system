/*
 * tweaks_dma_sdio.c
 *
 *  Created on: 2 дек. 2016 г.
 *      Author: dalexies
 */

#include "utils_driver_sd.h"

#include "dma.h"
#include "cmsis_os.h"

#ifndef USE_STDPERIPH_SDCARD
extern DMA_HandleTypeDef hdma_sdio;
extern SD_HandleTypeDef hsd;
extern HAL_SD_CardInfoTypedef SDCardInfo;
#endif

xSemaphoreHandle sdcard_mutex = NULL;

#ifndef USE_STDPERIPH_SDCARD
void TWEAK_Set_DMA_SDIO_Direction(TWEAK_DMA_SDIO_Direction direction)
{
	switch (direction)
	{
	case TWEAK_DMA_SDIO_DIR_TO_DEVICE:
		hdma_sdio.Init.Direction = DMA_MEMORY_TO_PERIPH;
		break;
	default:
	case TWEAK_DMA_SDIO_DIR_TO_MEMORY:
		hdma_sdio.Init.Direction = DMA_PERIPH_TO_MEMORY;
		break;
	}

	if (HAL_DMA_Init(&hdma_sdio) != HAL_OK)
	{
	    Error_Handler();
	}
    /* SDIO_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SDIO_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
    /* DMA2_Channel4_5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Channel4_5_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);
    //__HAL_LINKDMA(&hsd,hdmarx,hdma_sdio);
    //__HAL_LINKDMA(&hsd,hdmatx,hdma_sdio);

    /* HAL SD initialization */
    //HAL_SD_Init(&hsd, &SDCardInfo);
}
#endif

void sdcard_mutex_lock()
{
    if (!sdcard_mutex)
        sdcard_mutex = xSemaphoreCreateMutex();
    xSemaphoreTake( sdcard_mutex, portMAX_DELAY);
}

void sdcard_mutex_unlock()
{
    xSemaphoreGive( sdcard_mutex );
}

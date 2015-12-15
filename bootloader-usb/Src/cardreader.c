/*
 * cardreader.c
 *
 *  Created on: 15 дек. 2015 г.
 *      Author: alexey
 */

#include "cardreader.h"
#include "sd-utils.h"
#include "bsp_driver_sd.h"
#include "usb_device.h"

extern HAL_SD_CardInfoTypedef SDCardInfo;
extern SD_HandleTypeDef hsd;

void initCardreader()
{
	MX_SDIO_SD_Init();
	BSP_SD_Init();
	MX_USB_DEVICE_Init();
	tmpCheckRes(HAL_SD_Get_CardInfo(&hsd, &SDCardInfo));
	printf("SDCardInfo.CardCapacity = %d\n", (int) SDCardInfo.CardCapacity);
	printf("SDCardInfo.CardBlockSize = %d\n", (int) SDCardInfo.CardBlockSize);
	printf("SDCardInfo.CardType = %d\n", (int) SDCardInfo.CardType);
}

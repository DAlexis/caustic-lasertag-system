/**
  ******************************************************************************
  * @file    sd_diskio.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    20-November-2014
  * @brief   SD Disk I/O driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Block Size in Bytes */
#define BLOCK_SIZE                512

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "diskio.h"
#include "ff_gen_drv.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
DSTATUS SD_initialize (void);
DSTATUS SD_status (void);
DRESULT SD_read (BYTE*, DWORD, UINT);
#if _USE_WRITE == 1
  DRESULT SD_write (const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT SD_ioctl (BYTE, void*);
#endif  /* _USE_IOCTL == 1 */
  
Diskio_drvTypeDef  SD_Driver =
{
  SD_initialize,
  SD_status,
  SD_read, 
#if  _USE_WRITE == 1
  SD_write,
#endif /* _USE_WRITE == 1 */
  
#if  _USE_IOCTL == 1
  SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/
#ifndef USE_STDPERIPH_SDCARD

// ////////////////////////////////////////
// SD-card driver using STM32 HAL //
// ////////////////////////////////////////

/**
  * @brief  Initializes a Drive
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_initialize(void)
{
  Stat = STA_NOINIT;
  
  /* Configure the uSD device */
  if(BSP_SD_Init() == MSD_OK)
  {
    Stat &= ~STA_NOINIT;
  }

  return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_status(void)
{
  Stat = STA_NOINIT;

  if(BSP_SD_GetStatus() == MSD_OK)
  {
    Stat &= ~STA_NOINIT;
  }
  
  return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT SD_read(BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_OK;
  
  if(BSP_SD_ReadBlocks((uint32_t*)buff, 
                       (uint64_t) (sector * BLOCK_SIZE), 
                       BLOCK_SIZE, 
                       count) != MSD_OK)
  {
    res = RES_ERROR;
  }
  
  return res;
}

/**
  * @brief  Writes Sector(s)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT SD_write(const BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_OK;
  
  if(BSP_SD_WriteBlocks((uint32_t*)buff,
                        (uint64_t)(sector * BLOCK_SIZE), 
                        BLOCK_SIZE, count) != MSD_OK)
  {
    res = RES_ERROR;
  }
  
  return res;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  SD_CardInfo CardInfo;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
  
  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    BSP_SD_GetCardInfo(&CardInfo);
    *(DWORD*)buff = CardInfo.CardCapacity / BLOCK_SIZE;
    res = RES_OK;
    break;
  
  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    *(WORD*)buff = BLOCK_SIZE;
    res = RES_OK;
    break;
  
  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD*)buff = BLOCK_SIZE;
    break;
  
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */

#else // USE_STDPERIPH_SDCARD

// ////////////////////////////////////////
// SD-card driver using STM32 HAL //
// ////////////////////////////////////////


#include "sdcard.h"
#include <string.h>
#include "core/os-wrappers.hpp"

Mutex SDCardBusy;

/* Disk drives */
#define DRIVE_SDHC      0
#define DRIVE_EHCI      1

/* Disk constants */
#define SECTOR_SZ(drv)          ((drv)==DRIVE_SDHC ? 512 :      \
        ((drv)==DRIVE_EHCI ? usbstorage_GetSectorSize() : 0))

extern SD_CardInfo SDCardInfo;

/**
  * @brief  Initializes a Drive
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_initialize(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	if (SD_Init() != SD_OK)
		return RES_ERROR;
	return 0;
}

/**
  * @brief  Gets Disk Status
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_status(void)
{
	return SD_OK;
}

/**
  * @brief  Reads Sector(s)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT SD_read(BYTE *buff, DWORD sector, UINT count)
{
	ScopedLock<Mutex> lck(SDCardBusy);
	if(count==1)
	{
		/// @todo [low] Put critical section deeper (all crit. sections below)
		taskENTER_CRITICAL();
			SD_Error res = SD_ReadBlock(buff, sector*512, 512);
		taskEXIT_CRITICAL();
		if (SD_OK != res)
			return RES_ERROR;
	}
	else
	{
		taskENTER_CRITICAL();
			SD_Error res = SD_ReadMultiBlocks(buff, sector*512, 512, count);
		taskEXIT_CRITICAL();
		if (SD_OK != res)
			return RES_ERROR;
	}

	// First step waiting
	if (SD_OK != SD_WaitReadOperation())
		return RES_ERROR;

	// Second step waiting
	SDTransferState transferState = SD_GetStatus();
	while(transferState != SD_TRANSFER_OK)
	{
		if (SD_TRANSFER_ERROR == transferState)
			return RES_ERROR;
		Kernel::yield();//taskYIELD();
		transferState = SD_GetStatus();
	}
	return RES_OK;
}

/**
  * @brief  Writes Sector(s)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT SD_write(const BYTE *buff, DWORD sector, UINT count)
{
	ScopedLock<Mutex> lck(SDCardBusy);
	if(count==1)
	{
		taskENTER_CRITICAL();
			SD_Error res = SD_WriteBlock((BYTE*)buff, sector*512, 512);
		taskEXIT_CRITICAL();
		if (SD_OK != res)
			return RES_ERROR;
	}
	else
	{
		taskENTER_CRITICAL();
			SD_Error res = SD_WriteMultiBlocks((BYTE*)buff, sector*512, 512, count);
		taskEXIT_CRITICAL();
		if (SD_OK != res)
			return RES_ERROR;
	}

	// First step waiting
	if (SD_OK != SD_WaitWriteOperation())
		return RES_ERROR;

	// Second step waiting
	SDTransferState transferState = SD_GetStatus();
	while(transferState != SD_TRANSFER_OK)
	{
		if (SD_TRANSFER_ERROR == transferState)
			return RES_ERROR;
		Kernel::yield();//taskYIELD();
		transferState = SD_GetStatus();
	}
	return RES_OK;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE cmd, void *buff)
{
	DRESULT res;

	switch(cmd)
	{
	case CTRL_SYNC:
		{
			ScopedLock<Mutex> lck(SDCardBusy);
			while(SDIO_GetResponse(SDIO_RESP1)==0);//(SD_WaitReady
	//        {
				res = RES_OK;
	//        }
	//        else
	//        {
	//            res = RES_ERROR;
	//        }
		}
		break;
	case GET_SECTOR_SIZE:
		*(WORD*)buff = 512;
		res = RES_OK;
		break;
	case GET_SECTOR_COUNT:
		if((SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1) || (SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0))
			*(DWORD*)buff = SDCardInfo.CardCapacity >> 9;
		else if(SDCardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)
			*(DWORD*)buff = (SDCardInfo.SD_csd.DeviceSize+1)*1024;
		//else;////SD_GetCapacity();
		res = RES_OK;
		break;
	case GET_BLOCK_SIZE:
		//*(WORD*)buff = SDCardInfo.CardBlockSize;
		*(WORD*)buff = 512;//SDCardInfo.CardBlockSize;
		res = RES_OK;
		break;

	default:
		res = RES_PARERR;
		break;
	}
	return res;
}
#endif /* _USE_IOCTL == 1 */

#endif // USE_STDPERIPH_SDCARD
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "hal/ff/diskio.h"		/* FatFs lower layer API */
#include "hw/sdcard.h"
//#include "spi.hpp"
#include <stdio.h>
#include <string.h>

//unsigned char sdcCommand[6];
#define SECTOR_SIZE 512U

/* Disk drives */
#define DRIVE_SDHC      0
#define DRIVE_EHCI      1

/* Disk constants */
#define SECTOR_SZ(drv)          ((drv)==DRIVE_SDHC ? 512 :      \
        ((drv)==DRIVE_EHCI ? usbstorage_GetSectorSize() : 0))


extern SD_CardInfo SDCardInfo;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	(void) pdrv;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	if (SD_Init() != SD_OK)
		return RES_ERROR;
/*
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = SD_SDIO_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init( &NVIC_InitStructure );
	*/
	//SD_EnableWideBusOperation(SDIO_BusWide_4b);
	/*
	SD_Init();
	// Получаем информацию о карте
	SD_GetCardInfo(&SDCardInfo);
	// Выбор карты и настройка режима работы
	SD_SelectDeselect((uint32_t) (SDCardInfo.RCA << 16));*/
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	if ( pdrv ) return STA_NOINIT; /* Supports only single drive */
	return SD_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	(void) pdrv;
	if(count==1)
	{
		SD_ReadBlock(buff, sector*512, 512);
		SD_WaitReadOperation();
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}
	else
	{
		SD_ReadMultiBlocks(buff, sector*512, 512, count);
		SD_WaitReadOperation();
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}
	/*
	if(count==1)
	{
		SD_Error qq=SD_ReadBlock(sector << 9, buff2, SECTOR_SIZE);
		if (qq != SD_OK)
			printf("not ok: %d\n", qq);
		memcpy(buff, buff2, SECTOR_SIZE);
	}
	else
	{
		SD_ReadMultiBlocks(sector << 9, buff2, SECTOR_SIZE, count);
		memcpy(buff, buff2, SECTOR_SIZE * count);
	}
*/
	return RES_OK;
}

//-----------------------------------------------------------------------
// Write Sector(s)
//-----------------------------------------------------------------------

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE* buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	(void)pdrv;
	if(count==1)
	{
		SD_WriteBlock((BYTE*)buff, sector*512, 512);
		SD_WaitWriteOperation();
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}
	else
	{
		SD_WriteMultiBlocks((BYTE*)buff, sector*512, 512, count);
		SD_WaitWriteOperation();
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}
	return RES_OK;

}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	if (pdrv)
	{
		return RES_PARERR;
	}

	switch(cmd)
	{
	case CTRL_SYNC:

		while(SDIO_GetResponse(SDIO_RESP1)==0);//(SD_WaitReady
//        {
			res = RES_OK;
//        }
//        else
//        {
//            res = RES_ERROR;
//        }
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
	return res;//RES_OK;//

	/*
	if (cmd == GET_SECTOR_SIZE) {
			s32 ret = *((WORD *) buff) = (WORD) SECTOR_SZ(pdrv);

			return (ret) ? RES_OK : RES_ERROR;
	}

	return RES_OK;
*/
	/*
	DRESULT res;
	int result;
	
	return RES_OK;*/
}
#endif

DWORD get_fattime (void)
{
	return 0;
}

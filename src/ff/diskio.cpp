/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "spi.hpp"
#include <stdio.h>

unsigned char sdcCommand[6];



void sdc_assert(void)
{
	GPIOA->ODR &= ~GPIO_Pin_4;
}

void sdc_deassert(void)
{
	GPIOA->ODR |= GPIO_Pin_4;
	SPI1Manager.send_single(0xFF); // send 8 clocks for SDC to set SDO tristate
}

uint8_t sdc_isConn(void)
{
	if (GPIOA->IDR & GPIO_Pin_8)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void sdc_sendCommand(uint8_t command, uint8_t par1, uint8_t par2, uint8_t par3, uint8_t par4)
{
	sdcCommand[0] = 0x40 | command;
	sdcCommand[1] = par1;
	sdcCommand[2] = par2;
	sdcCommand[3] = par3;
	sdcCommand[4] = par4;
	
	if (command == SDC_GO_IDLE_STATE)
	{
		sdcCommand[5] = 0x95; // precalculated CRC
	}
	else 
	{
		sdcCommand[5] = 0xFF;
	}
	SPI1Manager.send(sdcCommand, 6);
}

uint8_t sdc_getResponse(uint8_t response)
{
	for(uint8_t n = 0; n < 8; n++)
	{
	    uint8_t res = SPI1Manager.receive_single();
	    //printf("res=%d\n", res);
		if (res == response)
		{
			return 0;
		}
	}
	return 1;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{	
	// initialize chip select line
	// chip select is PA4
    //printf("Inside of disk_initialize\n");
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIOA->ODR |= GPIO_Pin_4;

	// initialize card detect line
	// card detect is PA8
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	//printf("SPI init...\n");

	SPI1Manager.init(SPI_BaudRatePrescaler_256);
	GPIOA->ODR |= GPIO_Pin_4;
	//printf("Connection test...\n");
	// check wether SDC is inserted into socket
	/*
	if (!sdc_isConn())
	{
		return STA_NODISK;
	}*/
	//printf("Connection test passed\n");
	// send 10 dummy bytes to wake up SDC
	//printf("Sending 10 0xFF's\n");
	for (uint8_t i = 0; i < 10; i++)
	{
		SPI1Manager.send_single(0xFF);
	}

	// assert SDC
	sdc_assert();
	// software reset the SDC
	sdc_sendCommand(SDC_GO_IDLE_STATE, 0, 0, 0, 0);

	// printf("Waiting response\n");
	if (sdc_getResponse(0x01))
	{
	    // printf("Bad response\n");
	    sdc_deassert();
		return STA_NOINIT;
	}
	// printf("Response test passed\n");

	// wait for SDC to come out of idle state
	uint8_t resp = 1;
	while(resp)
	{
		sdc_sendCommand(SDC_SEND_OP_COND, 0, 0, 0, 0);
		if (!sdc_getResponse(0x00))
		{
			resp = 0;
		}
	}
	// deassert the SDC
	sdc_deassert();

	SPI1Manager.init(SPI_BaudRatePrescaler_2);

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	unsigned char result[2];

	if (!sdc_isConn())
	{
		return STA_NODISK;
	}
	/*
	sdc_assert();
	sdc_sendCommand(SDC_SEND_STATUS, 0, 0, 0, 0);
	SPI1Manager.receive(result, 2);
	sdc_deassert();
	
	if (result[0] & 0x01)
	{
		return STA_NOINIT; 
	}
	else if (result[1] & 0x01)
	{
		return STA_PROTECT;
	}
	*/
	
	return 0;
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
	unsigned char result;
	unsigned char buf[2];
	
	// check if parameters are in valid range
	if( count > 128 )
	{
		return RES_PARERR;
	}
	
	sector *= 512; // convert LBA to physical address
	sdc_assert(); // assert SDC
	
	// if multiple sectors are to be read
	if(count > 1)
	{
		// start multiple sector read
		sdc_sendCommand(SDC_READ_MULTIPLE_BLOCK, ((sector>>24)&0xFF), ((sector>>16)&0xFF), ((sector>>8)&0xFF), (sector&0xFF));
		while(sdc_getResponse(0x00)); // wait for command acknowledgement
		
		while(count)
		{
			while(sdc_getResponse(0xFE)); // wait for data token 0xFE
			SPI1Manager.receive(buff, 512); // read 512 bytes
			SPI1Manager.receive(buf, 2); // receive two byte CRC
			count--;
			buff += 512;
		}
		// stop multiple sector read
		sdc_sendCommand(SDC_STOP_TRANSMISSION, 0, 0, 0, 0);
		while(sdc_getResponse(0x00)); // wait for R1 response
	}
	else // if single sector is to be read
	{
		sdc_sendCommand(SDC_READ_SINGLE_BLOCK, ((sector>>24)&0xFF), ((sector>>16)&0xFF), ((sector>>8)&0xFF), (sector&0xFF));
		while(sdc_getResponse(0x00)); // wait for command acknowledgement
		while(sdc_getResponse(0xFE)); // wait for data token 0xFE
		SPI1Manager.receive(buff, 512); // receive data
		SPI1Manager.receive(buf, 2); // receive two byte CRC
	}
	
	while(!SPI1Manager.receive_single()); // wait until card is not busy anymore
	
	sdc_deassert(); // deassert SDC 
	
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE* buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	unsigned char result;
	unsigned char buf[2];
	buf[0] = 0xFF;
	buf[1] = 0xFF;
	
	// check if parameters are in valid range
	if( count > 128 )
	{
		return RES_PARERR;
	}
	
	sector *= 512; // convert LBA to physical address
	sdc_assert(); // assert SDC
	
	// if multiple sectors are to be written
	if(count > 1)
	{
		// start multiple sector write
		sdc_sendCommand(SDC_WRITE_MULTIPLE_BLOCK, ((sector>>24)&0xFF), ((sector>>16)&0xFF), ((sector>>8)&0xFF), (sector&0xFF));
		while(sdc_getResponse(0x00)); // wait for R1 response
		SPI1Manager.send_single(0xFF);  // send one byte gap
		
		while(count)
		{
			SPI1Manager.send_single(0xFC); // send multi byte data token 0xFC
			SPI1Manager.send((unsigned char*)buff, 512); // send 512 bytes
			SPI1Manager.send(buf, 2); // send two byte CRC
			
			// check if card has accepted data
			result = SPI1Manager.receive_single();
			if( (result & 0x1F) != 0x05)
			{
				return RES_ERROR;
			}
			count--;
			buff += 512;
			while(!SPI1Manager.receive_single()); // wait until SD card is ready
		}
		
		SPI1Manager.send_single(0xFD); // send stop transmission data token 0xFD
		SPI1Manager.send_single(0xFF);  // send one byte gap
	}
	else // if single sector is to be written
	{
		sdc_sendCommand(SDC_WRITE_BLOCK, ((sector>>24)&0xFF), ((sector>>16)&0xFF), ((sector>>8)&0xFF), (sector&0xFF));
		while(sdc_getResponse(0x00)); // wait for R1 response
		SPI1Manager.send_single(0xFF);  // send one byte gap
		SPI1Manager.send_single(0xFE); // send data token 0xFE
		SPI1Manager.send((unsigned char*)buff, 512); // send data
		SPI1Manager.send(buf, 2); // send two byte CRC
		// check if card has accepted data
		result = SPI1Manager.receive_single();
		if( (result & 0x1F) != 0x05)
		{
			return RES_ERROR;
		}
	}
	
	while(!SPI1Manager.receive_single()); // wait until card is not busy anymore
	
	sdc_deassert(); // deassert SDC 
	
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
	int result;
	
	return RES_OK;
}
#endif

DWORD get_fattime (void)
{
	return 0;
}

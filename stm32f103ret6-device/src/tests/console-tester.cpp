/*
 * console-tester.cpp
 *
 *  Created on: 26 дек. 2014 г.
 *      Author: alexey
 */

#include "tests/console-tester.hpp"
#include "core/scheduler.hpp"
#include "hal/fire-emitter.hpp"
#include "hal/ff/ff.h"
#include "hal/fragment-player.hpp"
#include "hal/io-pins.hpp"
#include "hw/sdcard.h"
#include "dev/console.hpp"
#include "dev/wav-player.hpp"
#include "dev/sdcard-fs.hpp"
#include <stdio.h>
#include <string.h>

extern SD_CardInfo SDCardInfo;



#define WHERE_AM_I      {char tmp=0; extern char _Main_Stack_Limit; \
                         printf("pos:%p, free stack:%d\n", (void*)&tmp, (&tmp - &_Main_Stack_Limit)); }

ConsoleTester::ConsoleTester()
{
	Console::instance().registerCommand(
		"tpulse",
		"infinite test fire pulse modulated by 1000us packs",
		std::bind(&ConsoleTester::firePulseTest, this, std::placeholders::_1)
	);
	Console::instance().registerCommand(
		"tsd",
		"read text from 1.txt",
		std::bind(&ConsoleTester::SDReadingTest, this, std::placeholders::_1)
	);
	Console::instance().registerCommand(
		"tsdr",
		"Test simple data reading",
		std::bind(&ConsoleTester::readSDMBRTest, this, std::placeholders::_1)
	);
	Console::instance().registerCommand(
		"tfp",
		"Test fragment player",
		std::bind(&ConsoleTester::fragmentPlayerTest, this, std::placeholders::_1)
	);
	Console::instance().registerCommand(
		"tpsf",
		"Test play sound file",
		std::bind(&ConsoleTester::playSoundFile, this, std::placeholders::_1)
	);
	Console::instance().registerCommand(
		"sp",
		"Stop playing sound",
		std::bind(&ConsoleTester::stopPlaying, this, std::placeholders::_1)
	);
	Console::instance().registerCommand(
		"rinit",
		"Init nrf24l01 module",
		std::bind(&ConsoleTester::radioInit, this, std::placeholders::_1)
	);
	Console::instance().registerCommand(
		"ri",
		"Interrogate radio",
		std::bind(&ConsoleTester::interrogateRadio, this, std::placeholders::_1)
	);
	Console::instance().registerCommand(
		"rtx",
		"turn on radio regular TX",
		std::bind(&ConsoleTester::radioRegularTxOnTest, this, std::placeholders::_1)
	);
}

void ConsoleTester::firePulseTest(const char*)
{
	printf("Test pulse start\n");
	fireEmittersPool->getFireEmitter(0)->init();
	fireEmittersPool->getFireEmitter(0)->setChannel(3);
	fireEmittersPool->getFireEmitter(0)->setCallback(std::bind(&ConsoleTester::firePulseTestCallback, this, std::placeholders::_1));
	fireEmittersPool->getFireEmitter(0)->startImpulsePack(true, 600);
}

void ConsoleTester::firePulseTestCallback(bool state)
{
	fireEmittersPool->getFireEmitter(0)->startImpulsePack(!state, 600);
}

void ConsoleTester::SDReadingTest(const char*)
{

	printf("Mounting volume...\n");
	if (!SDCardFS::instance().init())
	{
		printf("Error during mounting sd-card!\n");
		return;
	}

	FRESULT res;
	//FATFS fatfs;
	FIL fil;
	char buffer[20];

	//f_mount(NULL, "", 1);


	//res = f_mount(&fatfs, "", 1); // mount the drive
	/*
	if (res)
	{
		printf("error %d occured!\n", res);
		return;
	} else {
		printf("success!\n");
	}*/

	printf("Opening file...\n");
	res = f_open(&fil, "1.txt", FA_OPEN_EXISTING | FA_WRITE | FA_READ); // open existing file in read and write mode
	if (res)
	{
		printf("error %d occured!\n", res);
		return;
	}
	printf("success!\n");
	UINT readed=0;
	f_read(&fil, buffer, 19, &readed);
	buffer[19] = '\0';
	printf("I read: \"%s\"\n", buffer);
	f_close(&fil);
	//f_mount(NULL, "", 1);
}

void ConsoleTester::readSDMBRTest(const char* arg)
{
	WHERE_AM_I;
	uint8_t *mbr = new uint8_t[512*4];
	int Status = SD_Init();
	if (Status == SD_OK) printf("SD_Init: Ok\n");
			else printf("SD_Init: %d\n", Status);

	NVIC_InitTypeDef NVIC_InitStructure;

	printf("Enabling interrupts\n");
	// Configure the NVIC Preemption Priority Bits
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	printf("SDCard block size: %d\n", (int)SDCardInfo.CardBlockSize);
	printf("SDCardInfo.CardCapacity = %u\n", SDCardInfo.CardCapacity);
	printf("SDCardInfo.RCA = %u\n", SDCardInfo.RCA);
	printf("SDCardInfo.CardType = %u\n", SDCardInfo.CardType);
	if (strcmp(arg, "1") == 0)
	{
		printf("Reading using SD_ReadBlock\n");
		Status = SD_ReadBlock(mbr, 0x00010e00, 512);
	}
	else
	{
		printf("Reading using SD_ReadMultiBlocks\n");
		SDIO_ClearITPendingBit(SDIO_IT_DATAEND);
		Status = SD_ReadMultiBlocks(mbr, 0x00010e00, 512, 4);
	}
	if (Status == SD_OK) printf("SD_ReadBlock/SD_ReadMultiBlocks: Ok\n");
			else printf("SD_ReadBlock/SD_ReadMultiBlocks: %d\n", Status);

	Status = SD_WaitReadOperation();

	if (Status == SD_OK) printf("SD_WaitReadOperation: Ok\n");
			else printf("SD_WaitReadOperation: %d\n", Status);

	for (int i=0; i<100; i+=4)
	{
		printf("%x %x %x %x\n", mbr[i], mbr[i+1], mbr[i+2], mbr[i+3]);
		if (SDIO_GetITStatus(SDIO_IT_DATAEND) != RESET)
				printf("7. Int flag set!\n");
	}

	printf("while(SD_GetStatus() != SD_TRANSFER_OK)\n");
	while(SD_GetStatus() != SD_TRANSFER_OK)
	{
		if (SD_GetStatus() == SD_TRANSFER_ERROR)
		{
			printf("Transfer error\n");
			break;
		}
	}
	printf("Done\n");

	delete[] mbr;
}

uint16_t buffer[] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

void ConsoleTester::fragmentPlayerTest(const char* arg)
{
	fragmentPlayer->setFragmentSize(11);
	if (strcmp(arg, "1") == 0)
		fragmentPlayer->setFragmentDoneCallback(nullptr);
	else
		fragmentPlayer->setFragmentDoneCallback(std::bind(&ConsoleTester::loadNextFragment, this, std::placeholders::_1));
	fragmentPlayer->playFragment(buffer);
}

void ConsoleTester::loadNextFragment(SoundSample* old)
{
	(void) old;
	fragmentPlayer->setFragmentSize(11);
	fragmentPlayer->playFragment(buffer);
}

void ConsoleTester::playSoundFile(const char* filename)
{
	if (filename[0] == '\0')
		filename = "sine.wav";
	WavPlayer::instance().setVerbose(false);
	WavPlayer::instance().loadAndPlay(filename);
}

void ConsoleTester::stopPlaying(const char*)
{
	WavPlayer::instance().stop();
}

void ConsoleTester::radioInit(const char*)
{
	if (!nrf)
		nrf = new NRF24L01Manager;
	//nrf->setDataReceiveCallback(std::bind(&ConsoleTester::radioRXCallback, this, std::placeholders::_1, std::placeholders::_2));
	nrf->init(
		IOPins->getIOPin(1, 7),
		IOPins->getIOPin(1, 12),
		IOPins->getIOPin(1, 8),
		nullptr,
		SPIs->getSPI(1)
	);
	nrf->printStatus();
	Scheduler::instance().addTask(std::bind(&NRF24L01Manager::interrogate, nrf), false, 10000);
}

void ConsoleTester::interrogateRadio(const char*)
{
	if (!nrf)
		radioInit("");
	nrf->interrogate();
	nrf->printStatus();
}

void ConsoleTester::radioRXCallback(uint8_t channel, uint8_t* data)
{
	printf("Data received on channel %u:\n", channel);
	printf("%x %x %x %x %x", data[0], data[1], data[2], data[3], data[4]);
}

void ConsoleTester::regularTX()
{
	unsigned char data[NRF24L01Manager::payloadSize];
	for (unsigned int i=0; i<NRF24L01Manager::payloadSize; i++)
		data[i] = i;
	nrf->sendData(NRF24L01Manager::payloadSize, data);
}

void ConsoleTester::radioRegularTxOnTest(const char*)
{
	printf("Sending every sec\n");
	Scheduler::instance().addTask(std::bind(&ConsoleTester::regularTX, this), false, 1000000);
}

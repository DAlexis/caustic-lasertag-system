/*
 * tester.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */


#include "tester.hpp"
#include "ff.h"
#include "console.hpp"
#include "sound.hpp"
#include "sdcard.hpp"
#include "radio.hpp"
#include "fire-led.hpp"
#include "miles-tag-2.hpp"

#include "memtest.h"

#include <stdio.h>
#include <malloc.h>

Tester tester;

Tester::Tester()
{
}

void Tester::init()
{
    registerCommands();
    radio.setDataReceiveCallback(RXCallback, 0);
    radio.setTXDoneCallback(TXDoneCallback, 0);
    radio.setTXMaxRetriesCallback(TXMaxRTCallback, 0);
}

void Tester::registerCommands()
{
    console.registerCommand("testsd", "test microSD card reading from file 1.txt", testSDCard);
    console.registerCommand("wav", "play sound from file", testSoundWav);
    console.registerCommand("stop", "stop sound plaing", stopSound);
    console.registerCommand("testmem", "test free mem amount", testFreeMem);
    console.registerCommand("tx", "test transmission", testTX);
    console.registerCommand("rs", "radio module status", radioStatus);
    console.registerCommand("ftx", "flush TX", flushTX);
    console.registerCommand("frx", "flush RX", flushRX);
    console.registerCommand("sf", "simple test for fire LED", simpleFireTest);
    console.registerCommand("ts", "make test shot", testShot);
    console.registerCommand("mtd", "y/n - enable/disable MT2 receiver debug", debugMT2Receiver);
}

void Tester::testSDCard(const char*)
{
    //WHERE_AM_I;
    FRESULT res;
    char buffer[6];
    //extern SDCardManager SDCard;

    if (!SDCard.mount())
    {
        printf("Failed");
        return;
    }

    printf("Opening file: \"1.txt\"...\n");
    res = f_open(SDCard.getDefaultFIL(), "1.txt", FA_OPEN_EXISTING | FA_WRITE | FA_READ); // open existing file in read and write mode
    if (res)
    {
        printf("error %d occured!\n", res);
        return;
    }
    printf("success!\n");
    //f_gets(buffer, 6, &fil);
    UINT readed=0;
    f_read(SDCard.getDefaultFIL(), buffer, 6, &readed);
    printf("I read: \"%s\"\n", buffer);
    //f_puts(buffer, &fil);
    f_close(SDCard.getDefaultFIL()); // close the file
    //SDCard.umount();
}

void Tester::testFreeMem(const char*)
{
    //printf("Testing memory\n");
    void* pointer = 0;
    int counter=0;
    do
    {
        pointer = malloc(10);
        if (pointer) counter++;
    } while (pointer != 0);

    printf("Allocated %d bytes\n", counter*10);
}

void Tester::testSoundWav(const char*)
{
    SDCard.mount();
    sound.playWav("piknik.wav");
    //sound.playWav("sine.wav");
    //SDCard.umount();
}

void Tester::stopSound(const char*)
{
    sound.stop();
}

void Tester::testTX(const char*)
{
    unsigned char data[32];
    for (unsigned int i=0; i<32; i++)
        data[i] = i;

    radio.sendData(32, data);
}

void Tester::radioStatus(const char*)
{
    radio.printStatus();
}

void Tester::flushRX(const char*)
{
    radio.flushRX();
}

void Tester::flushTX(const char*)
{
    radio.flushTX();
}

void Tester::RXCallback(void*, unsigned char channel, unsigned char* data)
{
    printf("Message on pipe %u: ", channel);
    for (int i=0; i<PAYLOAD_SIZE; i++)
        printf("%x ", data[i]);
    printf("\n");
}

void Tester::TXDoneCallback(void*)
{
    printf("TX done\n");
}

void Tester::TXMaxRTCallback(void*)
{
    printf("Max RT achieved!\n");
    radio.printStatus();
}

void Tester::simpleFireTest(const char*)
{
    fireLED.setCallback(fireLEDCallback, &tester);
    fireLED.startImpulsePack(true, 1000);
}

void Tester::debugMT2Receiver(const char *arg)
{
    if (arg[0] == 'y')
        milesTag2Receiver.enableDebug(true);
    else
        milesTag2Receiver.enableDebug(false);
}

void Tester::fireLEDCallback(void*, bool wasOnState)
{
    //printf("Alive\n");
    //if (wasOnState) printf("true\n");
    //else printf("false\n");
    fireLED.startImpulsePack(!wasOnState, 1000);
}

void Tester::testShot(const char*)
{
    milesTag2.init();
    milesTag2.shot(1);
}

void Tester::buttonCallback(void*, bool first)
{
    printf("Piu! ");
    if (first)
        printf("first=true\n");
    else
        printf("first=false\n");
    milesTag2.shot(1);
}


void Tester::mt2receiverCallback(void*, unsigned int teamId, unsigned int playerId, unsigned int damage)
{
    printf("Shot - teamId: %u, playerId: %u, damageId: %u\n", teamId, playerId, damage);
}

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
#include <stdio.h>
#include <malloc.h>

Tester tester;

void Tester::registerCommands()
{
    console.registerCommand("testsd", "test microSD card reading from file 1.txt", testSDCard);
    console.registerCommand("wav", "play sound from file", testSoundWav);
    console.registerCommand("stop", "stop sound plaing", stopSound);
    console.registerCommand("testmem", "test free mem amount", testFreeMem);
    console.registerCommand("txa", "read tx adress", readTXAdr);
    console.registerCommand("txw", "write tx adress", writeTXAdr);
}

void Tester::testSDCard(const char*)
{

    FIL fil;
    FRESULT res;
    char buffer[6];
    //extern SDCardManager SDCard;

    if (!SDCard.mount())
    {
        printf("Failed");
        return;
    }

    printf("Opening file: \"1.txt\"...\n");
    res = f_open(&fil, "1.txt", FA_OPEN_EXISTING | FA_WRITE | FA_READ); // open existing file in read and write mode
    if (res)
    {
        printf("error %d occured!\n", res);
        return;
    }
    printf("success!\n");
    //f_gets(buffer, 6, &fil);
    UINT readed=0;
    f_read(&fil, buffer, 6, &readed);
    printf("I read: \"%s\"\n", buffer);
    //f_puts(buffer, &fil);
    f_close(&fil); // close the file
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

void Tester::readTXAdr(const char*)
{
    radio.readTXAdress();
}

void Tester::writeTXAdr(const char*)
{
    radio.writeTXAdress();
}


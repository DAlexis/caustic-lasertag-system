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
#include <stdio.h>

Tester tester;

void runTestsd(const char*)
{
    tester.testSDCard();
}

void runSoundTestWav(const char* filename)
{
    tester.testSoundWav(filename);
}

void runTestFreeMem(const char*)
{
    tester.testFreeMem();
}

void registerTests()
{
    Console::getInstance().registerCommand("testsd", "test microSD card reading from file 1.txt", runTestsd);
    Console::getInstance().registerCommand("wav", "play sound from file", runSoundTestWav);
    Console::getInstance().registerCommand("testmem", "test free mem amount", runTestFreeMem);
}

void Tester::testSDCard()
{

    FIL fil;
    FRESULT res;
    char buffer[6];
    printf("SD-card initialization...\n");

/*
    FATFS FatFs;
    printf("Mounting volume...\n");
    res = f_mount(&FatFs, "", 1); // mount the drive
    if (res)
    {
        printf("error occured!\n");
        return;
    }
    printf("success!\n");
*/


    if (!SDCardManager::getInstance().mount())
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
    f_gets(buffer, 6, &fil);
    printf("I read: \"%s\"\n", buffer);
    f_puts(buffer, &fil);
    f_close(&fil); // close the file
    f_mount(NULL, "", 1);
}

void Tester::testFreeMem()
{
    printf("Testing memory\n");
    void* pointer;
    int counter=0;
    do
    {
        pointer = malloc(10);
    } while (pointer != 0);
    printf("Allocated %d bytes\n", counter*10);
}

void Tester::testSoundWav(const char* filename)
{
    SoundPlayer::getInstance().playWav(filename);
}

/*
 * tester.cpp
 *
 *  Created on: 15 окт. 2014 г.
 *      Author: alexey
 */


#include "tester.hpp"
#include "ff.h"
#include "console.hpp"
#include <stdio.h>

Tester tester;

void runTestsd(const char*)
{
    tester.testSDCard();
}

void registerTests()
{
    Console::getInstance().registerCommand("testsd", "test microSD card reading from file 1.txt", runTestsd);
}

void Tester::testSDCard()
{
    FATFS FatFs;
    FIL fil;
    FRESULT res;
    char buffer[6];
    printf("SD-card initialization...\n");

    printf("Mounting volume...\n");
    res = f_mount(&FatFs, "", 1); // mount the drive
    if (res)
    {
        printf("error occured!\n");
        return;
    }
    printf("success!\n");

    printf("Opening file: \"1.txt\"...\n");
    res = f_open(&fil, "1.txt", FA_OPEN_EXISTING | FA_WRITE | FA_READ); // open existing file in read and write mode
    if (res)
    {
        printf("error occured!\n");
        return;
    }
    printf("success!\n");
    f_gets(buffer, 6, &fil);
    printf("I read: \"%s\"\n", buffer);
    f_puts(buffer, &fil);
    f_close(&fil); // close the file
    f_mount(NULL, "", 1);
}

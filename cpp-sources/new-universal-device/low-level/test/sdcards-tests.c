/*
 * sdcards-tests.c
 *
 *  Created on: 3 дек. 2016 г.
 *      Author: dalexies
 */

#include "sdcard-tests.h"
#include "ff.h"
#include "bsp_driver_sd.h"

#include <string.h>
#include <stdint.h>

#define TEST_BUFFER_SIZE    20
#define TEST_BLOCK_NUMBER   12582912

FATFS fs;
FIL f;
char buffer[TEST_BUFFER_SIZE];

uint8_t test_block[512];

int test_mount()
{
    printf("- test: fs mounting\n");
    FRESULT res = f_mount(&fs, "", 1);
    if (res == FR_OK)
    {
        printf("Filesystem mounted successfuly.\n");
        return 1;
    } else {
        printf("Filesystem mounting error, f_mount returned: %d\n", (int)res);
        return 0;
    }
}

int test_read_file(const char* name)
{
    printf("- test: file reading\n");
    FRESULT res = f_open(&f, name, FA_READ);
    if (res == FR_OK)
    {
        printf("File opened successfully.\n");
    } else {
        printf("File opening failed, f_open returned: %d\n", (int) res);
        return 0;
    }
    UINT br = 0;
    res = f_read(&f, buffer, TEST_BUFFER_SIZE-1, &br);
    if (res == FR_OK)
    {
        buffer[TEST_BUFFER_SIZE-1] = '\0';
        printf("File readed successfuly. Readed: %d, Content: %s.\n", (int) br, buffer);
    } else {
        printf("File reading failed. f_read returned: %d\n", (int) res);
    }
    res = f_close(&f);
    if (res == FR_OK)
    {
        printf("File closed successfuly.\n");
    } else {
        printf("File closing failed. f_close returned: %d\n", (int) res);
        return 0;
    }
    return 1;
}

int test_write_file(const char* name)
{
    printf("- test: file writing\n");
    FRESULT res = f_open(&f, name, FA_WRITE | FA_CREATE_ALWAYS);
    if (res == FR_OK)
    {
        printf("File opened successfully.\n");
    } else {
        printf("File opening failed, f_open returned: %d\n", (int) res);
        return 0;
    }
    UINT bw = 0;
    const char sample[] = "Hello, file writing";
    int real_size = strlen(sample) + 1;
    res = f_write(&f, sample, real_size, &bw);
    if (res == FR_OK)
    {
        buffer[TEST_BUFFER_SIZE-1] = '\0';
        printf("File written successfuly. Written: %d/%d, Content: %s.\n", (int) bw, (int) real_size, sample);
    } else {
        printf("File writing failed. f_write returned: %d\n", (int) res);
    }
    res = f_close(&f);
    if (res == FR_OK)
    {
        printf("File closed successfuly.\n");
    } else {
        printf("File closing failed. f_close returned: %d\n", (int) res);
        return 0;
    }
    return 1;
}

int test_block_write(Test_DMA_Settings dma)
{
    printf("Writing test block...\n");
    int res = 0;

    switch (dma) {
    case TEST_USE_DMA: res = BSP_SD_WriteBlocks_DMA((uint32_t*) test_block, TEST_BLOCK_NUMBER, 512, 1); break;
    case TEST_NO_DMA:  res = BSP_SD_WriteBlocks((uint32_t*) test_block, TEST_BLOCK_NUMBER, 512, 1);     break;
    };

    if (res == MSD_OK)
    {
        printf("Writing test block done successfuly.\n");
        return 1;
    } else {
        printf("Writing test block failed\n");
        return 0;
    }
}

int test_block_read(Test_DMA_Settings dma)
{
    printf("Reading test block...\n");
    int res = 0;

    switch(dma) {
    case TEST_USE_DMA: res = BSP_SD_ReadBlocks_DMA((uint32_t*) test_block, TEST_BLOCK_NUMBER, 512, 1); break;
    case TEST_NO_DMA:  res = BSP_SD_ReadBlocks((uint32_t*) test_block, TEST_BLOCK_NUMBER, 512, 1);     break;
    };

    if (res == MSD_OK)
    {
        printf("Reading test block done successfuly.\n");
        return 1;
    } else {
        printf("Reading test block failed\n");
        return 0;
    }
}

int test_block_read_and_write(Test_DMA_Settings dma)
{
    return test_block_read(dma) && test_block_write(dma);
}

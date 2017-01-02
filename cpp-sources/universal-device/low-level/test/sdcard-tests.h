/*
 * sdcard-tests.h
 *
 *  Created on: 3 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef NEW_UNIVERSAL_DEVICE_TEST_SDCARD_TESTS_H_
#define NEW_UNIVERSAL_DEVICE_TEST_SDCARD_TESTS_H_

typedef enum {
    TEST_USE_DMA = 0,
    TEST_NO_DMA
} Test_DMA_Settings;

int test_mount();
int test_read_file(const char* name);
int test_write_file(const char* name);

int test_block_write(Test_DMA_Settings dma);
int test_block_read(Test_DMA_Settings dma);
int test_block_read_and_write(Test_DMA_Settings dma);

#endif /* NEW_UNIVERSAL_DEVICE_TEST_SDCARD_TESTS_H_ */

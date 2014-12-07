//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "stm32f10x.h"
#include "alive-indicator.hpp"
#include "usart.hpp"
#include "spi.hpp"
#include "console.hpp"
#include "tester.hpp"
#include "sound.hpp"
#include "radio.hpp"
#include "fire-led.hpp"
#include "memtest.h"
#include "utils.hpp"
#include "miles-tag-2.hpp"
#include "buttons-manager.hpp"

#include <stdio.h>
#include "diag/Trace.h"
#include <stdlib.h>
#include <functional>

//volatile char array[7000] = "dasdasdasdads";

void mesureStack(unsigned int currentSize)
{
    volatile char q[100]= "q";
    printf("%u, %s\n", currentSize, q);

    mesureStack(currentSize+100);
}

void mesureHeap()
{
    void* pointer = 0;
    void* prelast = 0;
    int counter=0;
    int step=10;
    do
    {
        prelast = pointer;
        pointer = malloc(step);
        if (pointer) counter++;
    } while (pointer != 0);
    printf("Allocated %d bytes, end at %p\n", counter*step, prelast);
}

void printMemInfo()
{

    WHERE_AM_I;
    extern char _Heap_Begin; // Defined by the linker.
    extern char _Heap_Limit;
    extern char  __stack;
    extern char _Main_Stack_Limit;
    printf("_end_noinit=_Heap_Begin: %p, __stack -__Main_Stack_Size=heap limit: %p, size=%d\n", (void*) &_Heap_Begin, (void*) &_Heap_Limit, &_Heap_Limit-&_Heap_Begin );
    printf("__stack=%p, _Main_Stack_Limit=%p\n", (void*) &__stack, (void*)&_Main_Stack_Limit);
}

void test(const char* w)
{
    printf("Hlo, %s\n", w);
}

int main()
{
    // At this stage the system clock should have already been configured
    // at high speed.
    //__enable_irq();
    AliveIndicator alive;
    //USART1Manager.init(9600);
    USART1Manager.init(921600);

    console.init(USART1Manager);

/*
    char* pb0 = (char*) malloc(1);
    char* pb1 = (char*) malloc(1);
    char* pb2 = (char*) malloc(1);
    mesureHeap();
    printf("0: %p, 1: %p, 2: %p\n", (void*) pb0, (void*) pb1, (void*) pb2);
    printf("hb-0: %d, 0-1: %d, 1-2: %d\n",
            pb0-&_Heap_Begin,
            pb1-pb0,
            pb2-pb1);

*/

    sound.init();
    systemTimer.delay(200000);
    radio.init();
    fireLED.init();
    milesTag2.init();
    milesTag2Receiver.init(4);
    milesTag2Receiver.setShortMessageCallback(Tester::mt2receiverCallback, nullptr);

    printMemInfo();

/*    std::function<void(void)> func1(test);
    func1();*/
    //systemTimer.delay_async(1000, test, "tester");
    //printf("sizeof(uint16_t)=%d, sizeof(int16_t)=%d\n", sizeof(uint16_t), sizeof(int16_t));
    tester.init();
    radio.setDataReceiveCallback(tester.RXCallback, 0);

    buttons.setButtonCallback(0, Tester::buttonCallback, nullptr);
    buttons.configButton(0, ButtonsManager::BUTTON_AUTO_REPEAT_ENABLE, 500000);
    printf("Initialization done\n");
/*
    printf("3...\n");
    systemTimer.delay(1000000);
    printf("2...\n");
    systemTimer.delay(1000000);
    printf("1...\n");
    systemTimer.delay(1000000);
    */
    //mesureStack(0);

    console.prompt();
    uint32_t lastTime=0, time=0;
    while (1)
    {
        systemTimer.testTimeSmoothness();
        milesTag2Receiver.interrogate();
        buttons.interrogateAllButtons();
        alive.blink();
        //radio.testTX();
        //    printf("test\n");
    }
}

// ----------------------------------------------------------------------------
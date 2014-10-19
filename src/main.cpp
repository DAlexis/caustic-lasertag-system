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

#include <stdio.h>
#include "diag/Trace.h"
#include <stdlib.h>


// ----------------------------------------------------------------------------
//
// STM32F1 empty sample (trace via STDOUT).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the STDOUT output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

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


int main()
{
    // At this stage the system clock should have already been configured
    // at high speed.
    //__enable_irq();
    AliveIndicator alive;
    USART1Manager.init(9600);

    console.init(USART1Manager);


    //SPIManagersPool::getInstance().create(SPI1);
    tester.registerCommands();


    extern char _Heap_Begin; // Defined by the linker.
    extern char _Heap_Limit;
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
    printf("Heap begin: %p, heap limit: %p, size=%d\n", (void*) &_Heap_Begin, (void*) &_Heap_Limit, &_Heap_Limit-&_Heap_Begin );
    sound.init();
    radio.init();
    printf("sizeof(uint16_t)=%d, sizeof(int16_t)=%d\n", sizeof(uint16_t), sizeof(int16_t));
    //Tester::testSDCard("qqq");
    printf("Initialization done\n");

    //mesureStack(0);

    console.prompt();
    while (1)
    {
        alive.blink();

        //    printf("test\n");
    }
}

// ----------------------------------------------------------------------------

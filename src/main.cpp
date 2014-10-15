//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "stm32f10x.h"
#include "alive-indicator.hpp"
#include "usart-manager.hpp"
#include "console.hpp"

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

int main()
{
    // At this stage the system clock should have already been configured
    // at high speed.
    __enable_irq();
    AliveIndicator alive;
    USARTManagersPool::getInstance().createUsart(USART1, 9600);
    printf("Initialization done\n");
    Console::InitConsole(USARTManagersPool::getInstance().getUsart(USART1));
    Console::getInstance().prompt();
    while (1)
    {
        alive.blink();

        //    printf("test\n");
    }
}

// ----------------------------------------------------------------------------

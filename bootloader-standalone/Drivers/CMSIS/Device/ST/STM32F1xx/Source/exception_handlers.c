//
// This file is part of the µOS++ III distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "ExceptionHandlers.h"

// ----------------------------------------------------------------------------

void lowLewelSystemInit();

// ----------------------------------------------------------------------------
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
// ----------------------------------------------------------------------------

#if defined(DEBUG)

// The DEBUG version is not naked, to allow breakpoints at Reset_Handler
void __attribute__ ((noreturn))
Reset_Handler (void)
  {
	lowLewelSystemInit ();
  }

#else

// The Release version is optimised to a quick branch to _start.
void __attribute__ ((naked))
Reset_Handler(void)
{
	lowLewelSystemInit();
  /*asm volatile
  (
      " ldr     r0,=loaderMain \n"
      " bx      r0"
      :
      :
      :
  );*/
}

#endif

void __attribute__ ((weak))
NMI_Handler(void)
{
  while (1)
    {
    }
}

void __attribute__ ((weak))
HardFault_Handler(void)
{
  while (1)
    {
    }
}

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

void __attribute__ ((weak))
MemManage_Handler(void)
  {
    while (1)
      {
      }
  }

void __attribute__ ((weak))
BusFault_Handler(void)
  {
    while (1)
      {
      }
  }

void __attribute__ ((weak))
UsageFault_Handler(void)
  {
    while (1)
      {
      }
  }

#endif

void __attribute__ ((weak))
SVC_Handler(void)
{
  while (1)
    {
    }
}

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

void __attribute__ ((weak))
DebugMon_Handler(void)
{
  while (1)
    {
    }
}

#endif

void __attribute__ ((weak))
PendSV_Handler(void)
{
  while (1)
    {
    }
}

void __attribute__ ((weak))
SysTick_Handler(void)
{
  // DO NOT loop, just return.
  // Useful in case someone (like STM HAL) inadvertently enables SysTick.
  ;
}


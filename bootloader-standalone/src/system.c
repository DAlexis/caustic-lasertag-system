/*
 * system.c
 *
 *  Created on: 7 дек. 2015 г.
 *      Author: alexey
 */

#include "system.h"
#include "flash-consts.h"
#include "misc.h"

//#include "stm32f1xx.h"
//

// Begin address for the initialisation values of the .data section.
// defined in linker script
extern unsigned int _sidata;
// Begin address for the .data section; defined in linker script
extern unsigned int _sdata;
// End address for the .data section; defined in linker script
extern unsigned int _edata;

extern unsigned int _isr_real;

// Begin address for the .bss section; defined in linker script
extern unsigned int __bss_start__;
// End address for the .bss section; defined in linker script
extern unsigned int __bss_end__;

void main();

inline void
__attribute__((always_inline))
__initialize_data (unsigned int* from, unsigned int* region_begin,
		   unsigned int* region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  unsigned int *p = region_begin;
  while (p < region_end)
    *p++ = *from++;
}

inline void
__attribute__((always_inline))
__initialize_bss (unsigned int* region_begin, unsigned int* region_end)
{
  // Iterate and clear word by word.
  // It is assumed that the pointers are word aligned.
  unsigned int *p = region_begin;
  while (p < region_end)
    *p++ = 0;
}


void lowLewelSystemInit()
{

	SystemInit();

	// The current version of SystemInit() leaves the value of the clock
	// in a RAM variable (SystemCoreClock), which will be cleared shortly,
	// so it needs to be recomputed after the RAM initialisations
	// are completed.

#if defined(OS_INCLUDE_STARTUP_INIT_FP) || (defined (__VFP_FP__) && !defined (__SOFTFP__))

	// Normally FP init is done by SystemInit(). In case this is not done
	// there, it is possible to force its inclusion by defining
	// OS_INCLUDE_STARTUP_INIT_FP.

	// Enable the Cortex-M4 FPU only when -mfloat-abi=hard.
	// Code taken from Section 7.1, Cortex-M4 TRM (DDI0439C)

	// Set bits 20-23 to enable CP10 and CP11 coprocessor
	SCB->CPACR |= (0xF << 20);

#endif // (__VFP_FP__) && !(__SOFTFP__)

	__initialize_data(&_sidata, &_sdata, &_edata);

	__initialize_bss(&__bss_start__, &__bss_end__);

	SystemCoreClockUpdate();

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, (uint32_t)&_isr_real - FLASH_BEGIN);
	main();
}


void moveISRForMainProgram()
{
	//__set_PRIMASK(1);
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xA000);
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, &_ISR_main);
	//__set_PRIMASK(0);
	//__enable_irq();
}

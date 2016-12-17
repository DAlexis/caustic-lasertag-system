#include "fault-handler.h"
#include "stm32f1xx_hal.h"

#include <stdint.h>

void printUsageErrorMsg(uint32_t CFSRValue)
{
   printf("Usage fault: \n");
   CFSRValue >>= 16;                  // right shift to lsb
   if((CFSRValue & (1 << 9)) != 0) {
      printf("Divide by zero\n");
   }
}

void printBusFaultErrorMsg(uint32_t CFSRValue)
{
   printf("Bus fault: ");
   CFSRValue = ((CFSRValue & 0x0000FF00) >> 8); // mask and right shift to lsb
}

void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
   printf("Memory Management fault: ");
   CFSRValue &= 0x000000FF; // mask just mem faults
}

enum { r0, r1, r2, r3, r12, lr, pc, psr};

void stackDump(uint32_t stack[])
{
   printf("r0  = 0x%08x\n", stack[r0]);
   printf("r1  = 0x%08x\n", stack[r1]);
   printf("r2  = 0x%08x\n", stack[r2]);
   printf("r3  = 0x%08x\n", stack[r3]);
   printf("r12 = 0x%08x\n", stack[r12]);
   printf("lr  = 0x%08x\n", stack[lr]);
   printf("pc  = 0x%08x\n", stack[pc]);
   printf("psr = 0x%08x\n", stack[psr]);

}

void onSystemFault()
{
    // We have HardFault interrupt or another very bad thing.
    printf("\n");
    printf("=====================================\n");
    printf("Strong system fault. He is dead, Jim.\n");
    printf("SCB->HFSR = 0x%08x\n", SCB->HFSR);
    if ((SCB->HFSR & SCB_HFSR_FORCED) != 0) {
       printf("Forced Hard Fault\n");
       printf("SCB->CFSR = 0x%08x\n", SCB->CFSR);
       if((SCB->CFSR & 0xFFFF0000) != 0) {
          printUsageErrorMsg(SCB->CFSR);
       }
       if((SCB->CFSR & 0xFF00) != 0) {
          printBusFaultErrorMsg(SCB->CFSR);
       }
       if((SCB->CFSR & 0xFF) != 0) {
          printMemoryManagementErrorMsg(SCB->CFSR);
       }
    }
    printf("=====================================\n");
    printf("\n");
#ifdef DEBUG
    for (;;) {}
#else
    NVIC_SystemReset();
#endif

}

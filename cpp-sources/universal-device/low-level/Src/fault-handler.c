#include "fault-handler.h"
#include "stm32f1xx_hal.h"

void printUsageErrorMsg(uint32_t CFSRValue)
{
   printf("Usage fault:\n");
   CFSRValue >>= 16;                  // right shift to lsb
   if((CFSRValue & (1 << 9)) != 0) {
      printf("Divide by zero\n");
   }
}

void printBusFaultErrorMsg(uint32_t CFSRValue)
{
   printf("Bus fault:\n");
   CFSRValue = (CFSRValue & 0x0000FF00); // mask

   if (CFSRValue & SCB_CFSR_DIVBYZERO)
           printf("DIV0: Divide-by-Zero Usage Fault\n");
   if (CFSRValue & SCB_CFSR_UNALIGNED)
       printf("UNALIGN: Unaligned Access Usage Fault\n");
   if (CFSRValue & SCB_CFSR_NOCP)
       printf("NOCP: No Coprocessor Usage Fault\n");
   if (CFSRValue & SCB_CFSR_INVPC)
       printf("INVCP: Invalid PC Load Usage Fault\n");
   if (CFSRValue & SCB_CFSR_INVSTATE)
       printf("INVSTAT: Invalid State Usage Fault\n");
   if (CFSRValue & SCB_CFSR_UNDEFINSTR)
       printf("UNDEF: Undefined Instruction Usage Fault\n");
   if (CFSRValue & SCB_CFSR_BFARVALID)
       printf("BFARV: Bus Fault Address Register Valid\n");
   if (CFSRValue & (1 << 13) )
       printf("BLSPERR: Bus Fault on Floating-Point Lazy State Preservation\n");
   if (CFSRValue & SCB_CFSR_STKERR)
       printf("BSTKE: Stack Bus Fault\n");
   if (CFSRValue & SCB_CFSR_UNSTKERR)
       printf("BUSTKE: Unstack Bus Fault. This is an asynchronous fault.\n");
   if (CFSRValue & SCB_CFSR_IMPRECISERR)
       printf("IMPRE: Imprecise Data Bus Error\n");
   if (CFSRValue & SCB_CFSR_PRECISERR)
       printf("PRECISE: Precise Data Bus Error\n");
   if (CFSRValue & SCB_CFSR_IBUSERR)
       printf("IBUS: Instruction Bus Error\n");

}

void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
    printf("Memory Management fault:\n");
    CFSRValue &= 0x000000FF; // mask just mem faults

    if (CFSRValue & SCB_CFSR_MMARVALID)
          printf("MMARV: Memory Management Fault Address Register Valid\n");
    if (CFSRValue & (1 << 5) )
        printf("MLSPERR: Memory Management Fault on Floating-Point Lazy State Preservation\n");
    if (CFSRValue & SCB_CFSR_MSTKERR)
        printf("MSTKE: Stack Access Violation\n");
    if (CFSRValue & SCB_CFSR_MUNSTKERR)
        printf("MUSTKE: Unstack Access Violation\n");
    if (CFSRValue & SCB_CFSR_DACCVIOL)
        printf("DERR: Data Access Violation\n");
    if (CFSRValue & SCB_CFSR_IACCVIOL)
        printf("SCB_CFSR_IACCVIOL = 1: processor attempted "
               "an instruction fetch from a location that does not permit execution\n");
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

void onSystemFault(uint32_t * hardfault_args)
{
    uint32_t stacked_r0;
    uint32_t stacked_r1;
    uint32_t stacked_r2;
    uint32_t stacked_r3;
    uint32_t stacked_r12;
    uint32_t stacked_lr;
    uint32_t stacked_pc;
    uint32_t stacked_psr;

    stacked_r0 = ( hardfault_args[0]);
    stacked_r1 = ( hardfault_args[1]);
    stacked_r2 = ( hardfault_args[2]);
    stacked_r3 = ( hardfault_args[3]);

    stacked_r12 = ( hardfault_args[4]);
    stacked_lr = ( hardfault_args[5]);
    stacked_pc = ( hardfault_args[6]);
    stacked_psr = ( hardfault_args[7]);

    printf ("=====================================\n");
    printf ("Strong system fault. He is dead, Jim.\n");
    printf ("R0 = %x\n", stacked_r0);
    printf ("R1 = %x\n", stacked_r1);
    printf ("R2 = %x\n", stacked_r2);
    printf ("R3 = %x\n", stacked_r3);
    printf ("R12 = %x\n", stacked_r12);
    printf ("LR [R14] = %x  subroutine call return address\n", stacked_lr);
    printf ("PC [R15] = %x  program counter\n", stacked_pc);
    printf ("PSR = %x\n", stacked_psr);
    printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));
    printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));
    printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));
    printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));
    printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));
    printf ("SCB_SHCSR = %x\n", SCB->SHCSR);
    // We have HardFault interrupt or another very bad thing.
    printf("\n");
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

#include "mcu-status.h"
#include "stm32f1xx_hal.h"

char isInsideInterrupt()
{
	return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ;
}

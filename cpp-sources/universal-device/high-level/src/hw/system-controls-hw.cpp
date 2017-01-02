#include "hw/system-controls-hw.hpp"
#include "stm32f1xx_hal.h"

static SystemControls systemControlsIstance;
ISystemControls* systemControls = &systemControlsIstance;

void SystemControls::rebootMCU()
{
    NVIC_SystemReset();
}

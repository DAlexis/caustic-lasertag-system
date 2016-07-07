cmake_minimum_required(VERSION 2.8)

project(universal-device C CXX ASM)

set(EXE_SOURCES
    Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xe.asm

    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sd.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_sdmmc.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    Drivers/newlib/_sbrk.c
    Drivers/newlib/_exit.c
    Drivers/stm32f1-stdperiph/src/stm32f10x_gpio.c
    Drivers/stm32f1-stdperiph/src/stm32f10x_dac.c
    Drivers/stm32f1-stdperiph/src/stm32f10x_tim.c
    Drivers/stm32f1-stdperiph/src/stm32f10x_exti.c
    Drivers/stm32f1-stdperiph/src/misc.c
    Drivers/stm32f1-stdperiph/src/stm32f10x_rcc.c
    Drivers/stm32f1-stdperiph/src/stm32f10x_sdio.c
    Drivers/stm32f1-stdperiph/src/stm32f10x_dma.c
    Drivers/stm32f1-stdperiph/src/stm32f10x_spi.c
    Drivers/stm32f1-stdperiph/src/stm32f10x_usart.c
    Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
    Middlewares/Third_Party/FatFs/src/option/syscall.c
    Middlewares/Third_Party/FatFs/src/option/ccsbcs.c
    Middlewares/Third_Party/FatFs/src/ff.c
    Middlewares/Third_Party/FatFs/src/diskio.c
    Middlewares/Third_Party/FatFs/src/ff_gen_drv.c
    Middlewares/Third_Party/glcd/graphs.c
    Middlewares/Third_Party/glcd/text.c
    Middlewares/Third_Party/glcd/graphics.c
    Middlewares/Third_Party/glcd/text_tiny.c
    Middlewares/Third_Party/glcd/devices/STM32F10x.c
    Middlewares/Third_Party/glcd/controllers/PCD8544.c
    Middlewares/Third_Party/glcd/controllers/NT75451.c
    Middlewares/Third_Party/glcd/controllers/ST7565R.c
    Middlewares/Third_Party/glcd/glcd.c
    Middlewares/Third_Party/glcd/unit_tests.c
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
    Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    Middlewares/Third_Party/FreeRTOS/Source/timers.c
    Middlewares/Third_Party/FreeRTOS/Source/list.c
    Middlewares/Third_Party/FreeRTOS/Source/queue.c
    Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    src/hw/freertos.c
    src/hw/stm32f1xx_hal_msp.c
    src/hw/bsp_driver_sd.c
    src/hw/stm32f1xx_it.c
    src/hw/fatfs.c
    
    Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.cpp
    Middlewares/Third_Party/FatFs/src/sd-stdperiph/sdcard-low-level.cpp
    Middlewares/Third_Party/FatFs/src/sd-stdperiph/sdcard.cpp
    src/rifle/rifle.cpp
    src/rifle/MFRC522-wrapper.cpp
    src/rifle/rifle-display.cpp
    src/rifle/packages-timings.cpp
    src/player/player.cpp
    src/bluetooth-bridge/bluetooth-protocol.cpp
    src/bluetooth-bridge/bluetooth-bridge.cpp
    src/network/network-layer.cpp
    src/network/broadcast.cpp
    src/device/device.cpp
    src/dev/random.cpp
    src/dev/buttons.cpp
    src/dev/nrf24l01.cpp
    src/dev/wav-player.cpp
    src/dev/io-pins-utils.cpp
    src/dev/rgb-leds.cpp
    src/dev/miles-tag-2-transmitter.cpp
    src/dev/lcd5110.cpp
    src/dev/RC522.cpp
    src/hw/rtc-hw.cpp
    src/hw/adc-hw.cpp
    src/hw/fire-emitter.cpp
    src/hw/io-pins-hw.cpp
    src/hw/fragment-player.cpp
    src/hw/hardware-initializer-hw.cpp
    src/hw/spi.cpp
    src/hw/system-clock.cpp
    src/hw/uart.cpp
    src/utils/fatfs-utils.cpp
    src/utils/memory-utils.cpp
    src/ir/ir-presentation.cpp
    src/ir/ir-presentation-mt2.cpp
    src/ir/ir-physical-tv.cpp
    src/rscp/RCSP-stream.cpp
    src/rscp/RCSP-state-saver.cpp
    src/rscp/RCSP-aggregator.cpp
    src/game/game-log.cpp
    src/main.cpp
    src/core/logging.cpp
    src/core/diagnostic.cpp
    src/core/device-initializer.cpp
    src/core/string-utils.cpp
    src/core/power-monitor.cpp
    src/core/pinout.cpp
    src/core/os-wrappers.cpp
    src/head-sensor/heand-sensor.cpp
    src/head-sensor/parameters.cpp
    src/head-sensor/head-sensor-base-types.cpp
)

include_directories(
    Drivers/stm32f1-stdperiph/include
    Drivers/STM32F1xx_HAL_Driver/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F1xx/Include
    Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc
    Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    Middlewares/Third_Party/FatFs/src/sd-stdperiph
    Middlewares/Third_Party/FatFs/src
    Middlewares/Third_Party/FatFs/src/drivers
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3
    Middlewares/Third_Party/glcd/devices/inc
    Middlewares/Third_Party/glcd
    include/system-config
    include
    
)

set(ELF_NAME ${PROJECT_NAME}.elf)
set(BIN_NAME ${PROJECT_NAME}.bin)
set(MAP_NAME ${PROJECT_NAME}.map)

# -mcpu=cortex-m3 -mthumb -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra -g3 -std=c++0x -g -fno-move-loop-invariants
# -mcpu=cortex-m3 -mthumb -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DUSE_FULL_ASSERT -DHSE_VALUE=8000000 -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -MMD -MP -MF"src/head-sensor/heand-sensor.d" -MT"src/head-sensor/heand-sensor.o" -c -o "src/head-sensor/heand-sensor.o" "../src/head-sensor/heand-sensor.cpp"


add_definitions(-DGLCD_DEVICE_STM32F10X)
add_definitions(-DGLCD_CONTROLLER_PCD8544)
add_definitions(-DUSE_STDPERIPH_DRIVER)
add_definitions(-DUSE_STDPERIPH_SDCARD)


set(LDSCRIPTS "-T STM32F103RETx_FLASH.ld")
set(LDSCRIPTS_DIR "${PROJECT_SOURCE_DIR}/ldscripts")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDSCRIPTS} -Xlinker --gc-sections -L${LDSCRIPTS_DIR} -Wl,-Map,\"${MAP_NAME}\" --specs=nano.specs")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles")

add_executable(${ELF_NAME} ${EXE_SOURCES})

add_custom_command(TARGET ${ELF_NAME}
                   POST_BUILD
                   COMMAND arm-none-eabi-objcopy ARGS -O binary ${ELF_NAME}  ${BIN_NAME}
                   )


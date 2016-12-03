cmake_minimum_required(VERSION 2.8)

project(new-universal-device C CXX ASM)

FILE(GLOB_RECURSE ASM_SOURCES *.s)
FILE(GLOB_RECURSE C_SOURCES *.c)
FILE(GLOB_RECURSE CPP_SOURCES *.cpp)
FILE(GLOB_RECURSE H_HEADERS *.h)
FILE(GLOB_RECURSE HPP_HEADERS *.hpp)

set(EXE_SOURCES
    ${ASM_SOURCES}
    ${C_SOURCES}
    ${CPP_SOURCES}
    ${H_HEADERS}
    ${HPP_HEADERS}
)

include_directories(
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F1xx/Include
    Drivers/STM32F1xx_HAL_Driver/Inc
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3
    Middlewares/Third_Party/FatFs/src
    Middlewares/Third_Party/FatFs/src/drivers
    Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    Inc
    test
)

set(ELF_NAME ${PROJECT_NAME}.elf)
set(BIN_NAME ${PROJECT_NAME}.bin)
set(MAP_NAME ${PROJECT_NAME}.map)


add_definitions(-DGLCD_DEVICE_STM32F10X)
add_definitions(-DGLCD_CONTROLLER_PCD8544)
add_definitions(-DUSE_STDPERIPH_DRIVER)
add_definitions(-DUSE_STDPERIPH_SDCARD)


set(LDSCRIPTS "-T STM32F103RETx_FLASH.ld")
set(LDSCRIPTS_DIR "${PROJECT_SOURCE_DIR}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDSCRIPTS} -Xlinker --gc-sections -L${LDSCRIPTS_DIR} -Wl,-Map,\"${MAP_NAME}\" --specs=nano.specs")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles")

add_executable(${ELF_NAME} ${EXE_SOURCES})

add_custom_command(TARGET ${ELF_NAME}
                   POST_BUILD
                   COMMAND arm-none-eabi-objcopy ARGS -O binary ${ELF_NAME}  ${BIN_NAME}
                   )


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/hw/ext-interrupts.cpp \
../src/hw/fire-emitter.cpp \
../src/hw/fragment-player.cpp \
../src/hw/io-pins.cpp \
../src/hw/leds.cpp \
../src/hw/sdcard-low-level.cpp \
../src/hw/sdcard.cpp \
../src/hw/spi.cpp \
../src/hw/system-clock.cpp \
../src/hw/usart.cpp 

OBJS += \
./src/hw/ext-interrupts.o \
./src/hw/fire-emitter.o \
./src/hw/fragment-player.o \
./src/hw/io-pins.o \
./src/hw/leds.o \
./src/hw/sdcard-low-level.o \
./src/hw/sdcard.o \
./src/hw/spi.o \
./src/hw/system-clock.o \
./src/hw/usart.o 

CPP_DEPS += \
./src/hw/ext-interrupts.d \
./src/hw/fire-emitter.d \
./src/hw/fragment-player.d \
./src/hw/io-pins.d \
./src/hw/leds.d \
./src/hw/sdcard-low-level.d \
./src/hw/sdcard.d \
./src/hw/spi.d \
./src/hw/system-clock.d \
./src/hw/usart.d 


# Each subdirectory must supply rules for building sources it contributes
src/hw/%.o: ../src/hw/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -DOS_USE_TRACE_ITM -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -DDEVICE_SIMPLE_IRRC -I"/home/alexey/Projects/lazertag/lazertag-libraries" -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"/home/alexey/Projects/lazertag/lazertag-libraries/include" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



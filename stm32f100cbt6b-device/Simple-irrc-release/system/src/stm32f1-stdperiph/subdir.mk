################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/stm32f1-stdperiph/misc.c \
../system/src/stm32f1-stdperiph/stm32f10x_dac.c \
../system/src/stm32f1-stdperiph/stm32f10x_dma.c \
../system/src/stm32f1-stdperiph/stm32f10x_exti.c \
../system/src/stm32f1-stdperiph/stm32f10x_gpio.c \
../system/src/stm32f1-stdperiph/stm32f10x_rcc.c \
../system/src/stm32f1-stdperiph/stm32f10x_sdio.c \
../system/src/stm32f1-stdperiph/stm32f10x_spi.c \
../system/src/stm32f1-stdperiph/stm32f10x_tim.c \
../system/src/stm32f1-stdperiph/stm32f10x_usart.c 

OBJS += \
./system/src/stm32f1-stdperiph/misc.o \
./system/src/stm32f1-stdperiph/stm32f10x_dac.o \
./system/src/stm32f1-stdperiph/stm32f10x_dma.o \
./system/src/stm32f1-stdperiph/stm32f10x_exti.o \
./system/src/stm32f1-stdperiph/stm32f10x_gpio.o \
./system/src/stm32f1-stdperiph/stm32f10x_rcc.o \
./system/src/stm32f1-stdperiph/stm32f10x_sdio.o \
./system/src/stm32f1-stdperiph/stm32f10x_spi.o \
./system/src/stm32f1-stdperiph/stm32f10x_tim.o \
./system/src/stm32f1-stdperiph/stm32f10x_usart.o 

C_DEPS += \
./system/src/stm32f1-stdperiph/misc.d \
./system/src/stm32f1-stdperiph/stm32f10x_dac.d \
./system/src/stm32f1-stdperiph/stm32f10x_dma.d \
./system/src/stm32f1-stdperiph/stm32f10x_exti.d \
./system/src/stm32f1-stdperiph/stm32f10x_gpio.d \
./system/src/stm32f1-stdperiph/stm32f10x_rcc.d \
./system/src/stm32f1-stdperiph/stm32f10x_sdio.d \
./system/src/stm32f1-stdperiph/stm32f10x_spi.d \
./system/src/stm32f1-stdperiph/stm32f10x_tim.d \
./system/src/stm32f1-stdperiph/stm32f10x_usart.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/stm32f1-stdperiph/%.o: ../system/src/stm32f1-stdperiph/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -DOS_USE_TRACE_ITM -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -DDEVICE_SIMPLE_IRRC -I"/home/alexey/Projects/lazertag/lazertag-libraries" -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"/home/alexey/Projects/lazertag/lazertag-libraries/include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



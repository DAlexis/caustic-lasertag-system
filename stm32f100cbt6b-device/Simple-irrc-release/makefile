################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include system/src/stm32f1-stdperiph/subdir.mk
-include system/src/newlib/subdir.mk
-include system/src/diag/subdir.mk
-include system/src/cortexm/subdir.mk
-include system/src/cmsis/subdir.mk
-include src/hw/ff/subdir.mk
-include src/hw/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
stm32f100cbt6b-device.bin \

SECONDARY_SIZE += \
stm32f100cbt6b-device.siz \


# All Target
all: stm32f100cbt6b-device.elf secondary-outputs

dependents:
	-cd /home/alexey/Projects/lazertag/lazertag-libraries/Debug && $(MAKE) clean all

# Tool invocations
stm32f100cbt6b-device.elf: $(OBJS) $(USER_OBJS) /home/alexey/Projects/lazertag/lazertag-libraries/Debug/liblazertag-libraries.a
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g -T mem.ld -T libs.ld -T sections.ld -nostartfiles -Xlinker --gc-sections -L"/home/alexey/Projects/lazertag/lazertag-libraries/Debug" -L"../ldscripts" -Wl,-Map,"stm32f100cbt6b-device.map" --specs=nano.specs -u _printf_float -u _scanf_float -o "stm32f100cbt6b-device.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

stm32f100cbt6b-device.bin: stm32f100cbt6b-device.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary "stm32f100cbt6b-device.elf"  "stm32f100cbt6b-device.bin"
	@echo 'Finished building: $@'
	@echo ' '

stm32f100cbt6b-device.siz: stm32f100cbt6b-device.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "stm32f100cbt6b-device.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(C++_DEPS)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(CC_DEPS)$(SECONDARY_FLASH)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS)$(S_UPPER_DEPS) stm32f100cbt6b-device.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:
/home/alexey/Projects/lazertag/lazertag-libraries/Debug/liblazertag-libraries.a:

-include ../makefile.targets

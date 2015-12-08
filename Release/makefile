################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include UART/subdir.mk
-include Motor/subdir.mk
-include ADC_MCP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
MotorFaderSteuerung.lss \

FLASH_IMAGE += \
MotorFaderSteuerung.hex \

EEPROM_IMAGE += \
MotorFaderSteuerung.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: MotorFaderSteuerung.elf secondary-outputs

# Tool invocations
MotorFaderSteuerung.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,MotorFaderSteuerung.map -mmcu=atmega8 -o "MotorFaderSteuerung.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

MotorFaderSteuerung.lss: MotorFaderSteuerung.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S MotorFaderSteuerung.elf  >"MotorFaderSteuerung.lss"
	@echo 'Finished building: $@'
	@echo ' '

MotorFaderSteuerung.hex: MotorFaderSteuerung.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex MotorFaderSteuerung.elf  "MotorFaderSteuerung.hex"
	@echo 'Finished building: $@'
	@echo ' '

MotorFaderSteuerung.eep: MotorFaderSteuerung.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex MotorFaderSteuerung.elf  "MotorFaderSteuerung.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: MotorFaderSteuerung.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 MotorFaderSteuerung.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) MotorFaderSteuerung.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

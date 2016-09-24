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
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
Fader_Steuerung.lss \

FLASH_IMAGE += \
Fader_Steuerung.hex \

EEPROM_IMAGE += \
Fader_Steuerung.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: Fader_Steuerung.elf secondary-outputs

# Tool invocations
Fader_Steuerung.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Fader_Steuerung.map -mmcu=atmega8 -o "Fader_Steuerung.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Fader_Steuerung.lss: Fader_Steuerung.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Fader_Steuerung.elf  >"Fader_Steuerung.lss"
	@echo 'Finished building: $@'
	@echo ' '

Fader_Steuerung.hex: Fader_Steuerung.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Fader_Steuerung.elf  "Fader_Steuerung.hex"
	@echo 'Finished building: $@'
	@echo ' '

Fader_Steuerung.eep: Fader_Steuerung.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Fader_Steuerung.elf  "Fader_Steuerung.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Fader_Steuerung.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 Fader_Steuerung.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) Fader_Steuerung.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

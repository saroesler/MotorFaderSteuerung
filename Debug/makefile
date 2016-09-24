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

sizedummy: Fader_Steuerung.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 Fader_Steuerung.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(ELFS)$(OBJS)$(ASM_DEPS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) Fader_Steuerung.elf
	-@echo ' '

secondary-outputs: $(LSS) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

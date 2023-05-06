################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include example/src/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := LPC11C24_3D_GE_Design
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: LPC11C24_3D_GE_Design.axf

# Tool invocations
LPC11C24_3D_GE_Design.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/Users/ezgikaya/Documents/MCUXpressoIDE_11.7.0_9198/Ezgi_workspace/nxp_lpcxpresso_11c24_board_lib/Debug" -L"/Users/ezgikaya/Documents/MCUXpressoIDE_11.7.0_9198/Ezgi_workspace/lpc_chip_11cxx_lib/Debug" -Xlinker -Map="LPC11C24_3D_GE_Design.map" -Xlinker --cref -Xlinker -print-memory-usage -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T LPC11C24_3D_GE_Design_Debug.ld -o "LPC11C24_3D_GE_Design.axf" $(OBJS) $(USER_OBJS) $(LIBS) -lnxp_lpcxpresso_11c24_board_lib -llpc_chip_11cxx_lib
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) LPC11C24_3D_GE_Design.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "LPC11C24_3D_GE_Design.axf"; # arm-none-eabi-objcopy -O binary "LPC11C24_3D_GE_Design.axf" "LPC11C24_3D_GE_Design.bin" ; checksum -p LPC11C24_301 -d "LPC11C24_3D_GE_Design.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets

################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc11xx.c \
../example/src/lcd__api.c \
../example/src/main.c \
../example/src/sysinit.c 

C_DEPS += \
./example/src/cr_startup_lpc11xx.d \
./example/src/lcd__api.d \
./example/src/main.d \
./example/src/sysinit.d 

OBJS += \
./example/src/cr_startup_lpc11xx.o \
./example/src/lcd__api.o \
./example/src/main.o \
./example/src/sysinit.o 


# Each subdirectory must supply rules for building sources it contributes
example/src/cr_startup_lpc11xx.o: ../example/src/cr_startup_lpc11xx.c example/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__LPC11XX__ -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M0 -DSDK_DEBUGCONSOLE=0 -I"/Users/ezgikaya/Documents/MCUXpressoIDE_11.7.0_9198/Ezgi_workspace/LPC11C24_3D_GE_Design/example/inc" -I"/Users/ezgikaya/Documents/MCUXpressoIDE_11.7.0_9198/Ezgi_workspace/lpc_chip_11cxx_lib/inc" -I"/Users/ezgikaya/Documents/MCUXpressoIDE_11.7.0_9198/Ezgi_workspace/nxp_lpcxpresso_11c24_board_lib/inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

example/src/%.o: ../example/src/%.c example/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__LPC11XX__ -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M0 -DSDK_DEBUGCONSOLE=0 -I"/Users/ezgikaya/Documents/MCUXpressoIDE_11.7.0_9198/Ezgi_workspace/LPC11C24_3D_GE_Design/example/inc" -I"/Users/ezgikaya/Documents/MCUXpressoIDE_11.7.0_9198/Ezgi_workspace/lpc_chip_11cxx_lib/inc" -I"/Users/ezgikaya/Documents/MCUXpressoIDE_11.7.0_9198/Ezgi_workspace/nxp_lpcxpresso_11c24_board_lib/inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-example-2f-src

clean-example-2f-src:
	-$(RM) ./example/src/cr_startup_lpc11xx.d ./example/src/cr_startup_lpc11xx.o ./example/src/lcd__api.d ./example/src/lcd__api.o ./example/src/main.d ./example/src/main.o ./example/src/sysinit.d ./example/src/sysinit.o

.PHONY: clean-example-2f-src


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../external_eeprom.c \
../final_MC2.c \
../i2c.c \
../timer1_config.c \
../uart.c 

OBJS += \
./external_eeprom.o \
./final_MC2.o \
./i2c.o \
./timer1_config.o \
./uart.o 

C_DEPS += \
./external_eeprom.d \
./final_MC2.d \
./i2c.d \
./timer1_config.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega16 -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



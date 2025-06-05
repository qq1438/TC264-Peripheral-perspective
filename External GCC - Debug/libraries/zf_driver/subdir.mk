################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/zf_driver/zf_driver_adc.c \
../libraries/zf_driver/zf_driver_delay.c \
../libraries/zf_driver/zf_driver_dma.c \
../libraries/zf_driver/zf_driver_encoder.c \
../libraries/zf_driver/zf_driver_exti.c \
../libraries/zf_driver/zf_driver_flash.c \
../libraries/zf_driver/zf_driver_gpio.c \
../libraries/zf_driver/zf_driver_pit.c \
../libraries/zf_driver/zf_driver_pwm.c \
../libraries/zf_driver/zf_driver_soft_iic.c \
../libraries/zf_driver/zf_driver_soft_spi.c \
../libraries/zf_driver/zf_driver_spi.c \
../libraries/zf_driver/zf_driver_timer.c \
../libraries/zf_driver/zf_driver_uart.c 

C_DEPS += \
./libraries/zf_driver/zf_driver_adc.d \
./libraries/zf_driver/zf_driver_delay.d \
./libraries/zf_driver/zf_driver_dma.d \
./libraries/zf_driver/zf_driver_encoder.d \
./libraries/zf_driver/zf_driver_exti.d \
./libraries/zf_driver/zf_driver_flash.d \
./libraries/zf_driver/zf_driver_gpio.d \
./libraries/zf_driver/zf_driver_pit.d \
./libraries/zf_driver/zf_driver_pwm.d \
./libraries/zf_driver/zf_driver_soft_iic.d \
./libraries/zf_driver/zf_driver_soft_spi.d \
./libraries/zf_driver/zf_driver_spi.d \
./libraries/zf_driver/zf_driver_timer.d \
./libraries/zf_driver/zf_driver_uart.d 

OBJS += \
./libraries/zf_driver/zf_driver_adc.o \
./libraries/zf_driver/zf_driver_delay.o \
./libraries/zf_driver/zf_driver_dma.o \
./libraries/zf_driver/zf_driver_encoder.o \
./libraries/zf_driver/zf_driver_exti.o \
./libraries/zf_driver/zf_driver_flash.o \
./libraries/zf_driver/zf_driver_gpio.o \
./libraries/zf_driver/zf_driver_pit.o \
./libraries/zf_driver/zf_driver_pwm.o \
./libraries/zf_driver/zf_driver_soft_iic.o \
./libraries/zf_driver/zf_driver_soft_spi.o \
./libraries/zf_driver/zf_driver_spi.o \
./libraries/zf_driver/zf_driver_timer.o \
./libraries/zf_driver/zf_driver_uart.o 


# Each subdirectory must supply rules for building sources it contributes
libraries/zf_driver/%.o: ../libraries/zf_driver/%.c libraries/zf_driver/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-gcc -std=c99 "@F:/智能车摄像头/代码/前车/Front_Car_V2.4.4/External GCC - Debug/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O0 -g3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc161 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-libraries-2f-zf_driver

clean-libraries-2f-zf_driver:
	-$(RM) ./libraries/zf_driver/zf_driver_adc.d ./libraries/zf_driver/zf_driver_adc.o ./libraries/zf_driver/zf_driver_delay.d ./libraries/zf_driver/zf_driver_delay.o ./libraries/zf_driver/zf_driver_dma.d ./libraries/zf_driver/zf_driver_dma.o ./libraries/zf_driver/zf_driver_encoder.d ./libraries/zf_driver/zf_driver_encoder.o ./libraries/zf_driver/zf_driver_exti.d ./libraries/zf_driver/zf_driver_exti.o ./libraries/zf_driver/zf_driver_flash.d ./libraries/zf_driver/zf_driver_flash.o ./libraries/zf_driver/zf_driver_gpio.d ./libraries/zf_driver/zf_driver_gpio.o ./libraries/zf_driver/zf_driver_pit.d ./libraries/zf_driver/zf_driver_pit.o ./libraries/zf_driver/zf_driver_pwm.d ./libraries/zf_driver/zf_driver_pwm.o ./libraries/zf_driver/zf_driver_soft_iic.d ./libraries/zf_driver/zf_driver_soft_iic.o ./libraries/zf_driver/zf_driver_soft_spi.d ./libraries/zf_driver/zf_driver_soft_spi.o ./libraries/zf_driver/zf_driver_spi.d ./libraries/zf_driver/zf_driver_spi.o ./libraries/zf_driver/zf_driver_timer.d ./libraries/zf_driver/zf_driver_timer.o ./libraries/zf_driver/zf_driver_uart.d ./libraries/zf_driver/zf_driver_uart.o

.PHONY: clean-libraries-2f-zf_driver


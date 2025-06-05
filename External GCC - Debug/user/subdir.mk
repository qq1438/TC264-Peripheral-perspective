################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user/cpu0_main.c \
../user/cpu1_main.c \
../user/isr.c 

C_DEPS += \
./user/cpu0_main.d \
./user/cpu1_main.d \
./user/isr.d 

OBJS += \
./user/cpu0_main.o \
./user/cpu1_main.o \
./user/isr.o 


# Each subdirectory must supply rules for building sources it contributes
user/%.o: ../user/%.c user/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-gcc -std=c99 "@F:/智能车摄像头/代码/前车/Front_Car_V2.4.4/External GCC - Debug/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O0 -g3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc161 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-user

clean-user:
	-$(RM) ./user/cpu0_main.d ./user/cpu0_main.o ./user/cpu1_main.d ./user/cpu1_main.o ./user/isr.d ./user/isr.o

.PHONY: clean-user


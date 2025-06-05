################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/zf_common/zf_common_clock.c \
../libraries/zf_common/zf_common_debug.c \
../libraries/zf_common/zf_common_fifo.c \
../libraries/zf_common/zf_common_font.c \
../libraries/zf_common/zf_common_function.c \
../libraries/zf_common/zf_common_interrupt.c 

C_DEPS += \
./libraries/zf_common/zf_common_clock.d \
./libraries/zf_common/zf_common_debug.d \
./libraries/zf_common/zf_common_fifo.d \
./libraries/zf_common/zf_common_font.d \
./libraries/zf_common/zf_common_function.d \
./libraries/zf_common/zf_common_interrupt.d 

OBJS += \
./libraries/zf_common/zf_common_clock.o \
./libraries/zf_common/zf_common_debug.o \
./libraries/zf_common/zf_common_fifo.o \
./libraries/zf_common/zf_common_font.o \
./libraries/zf_common/zf_common_function.o \
./libraries/zf_common/zf_common_interrupt.o 


# Each subdirectory must supply rules for building sources it contributes
libraries/zf_common/%.o: ../libraries/zf_common/%.c libraries/zf_common/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-gcc -std=c99 "@F:/智能车摄像头/代码/前车/Front_Car_V2.4.4/External GCC - Debug/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O0 -g3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc161 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-libraries-2f-zf_common

clean-libraries-2f-zf_common:
	-$(RM) ./libraries/zf_common/zf_common_clock.d ./libraries/zf_common/zf_common_clock.o ./libraries/zf_common/zf_common_debug.d ./libraries/zf_common/zf_common_debug.o ./libraries/zf_common/zf_common_fifo.d ./libraries/zf_common/zf_common_fifo.o ./libraries/zf_common/zf_common_font.d ./libraries/zf_common/zf_common_font.o ./libraries/zf_common/zf_common_function.d ./libraries/zf_common/zf_common_function.o ./libraries/zf_common/zf_common_interrupt.d ./libraries/zf_common/zf_common_interrupt.o

.PHONY: clean-libraries-2f-zf_common


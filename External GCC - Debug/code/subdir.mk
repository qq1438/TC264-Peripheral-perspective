################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/All_Init.c \
../code/Filter.c \
../code/PID.c \
../code/PIT_Demo.c \
../code/PTS_Deal.c \
../code/Ring.c \
../code/Show_data.c \
../code/clip.c \
../code/cross.c \
../code/image_processing.c \
../code/key.c \
../code/menu.c \
../code/menu_list.c \
../code/parameter_flash.c \
../code/perspective.c \
../code/pico_gy.c \
../code/pico_link_ii.c \
../code/point_judge.c 

C_DEPS += \
./code/All_Init.d \
./code/Filter.d \
./code/PID.d \
./code/PIT_Demo.d \
./code/PTS_Deal.d \
./code/Ring.d \
./code/Show_data.d \
./code/clip.d \
./code/cross.d \
./code/image_processing.d \
./code/key.d \
./code/menu.d \
./code/menu_list.d \
./code/parameter_flash.d \
./code/perspective.d \
./code/pico_gy.d \
./code/pico_link_ii.d \
./code/point_judge.d 

OBJS += \
./code/All_Init.o \
./code/Filter.o \
./code/PID.o \
./code/PIT_Demo.o \
./code/PTS_Deal.o \
./code/Ring.o \
./code/Show_data.o \
./code/clip.o \
./code/cross.o \
./code/image_processing.o \
./code/key.o \
./code/menu.o \
./code/menu_list.o \
./code/parameter_flash.o \
./code/perspective.o \
./code/pico_gy.o \
./code/pico_link_ii.o \
./code/point_judge.o 


# Each subdirectory must supply rules for building sources it contributes
code/%.o: ../code/%.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-gcc -std=c99 "@F:/智能车摄像头/代码/前车/Front_Car_V2.4.4/External GCC - Debug/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O0 -g3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc161 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-code

clean-code:
	-$(RM) ./code/All_Init.d ./code/All_Init.o ./code/Filter.d ./code/Filter.o ./code/PID.d ./code/PID.o ./code/PIT_Demo.d ./code/PIT_Demo.o ./code/PTS_Deal.d ./code/PTS_Deal.o ./code/Ring.d ./code/Ring.o ./code/Show_data.d ./code/Show_data.o ./code/clip.d ./code/clip.o ./code/cross.d ./code/cross.o ./code/image_processing.d ./code/image_processing.o ./code/key.d ./code/key.o ./code/menu.d ./code/menu.o ./code/menu_list.d ./code/menu_list.o ./code/parameter_flash.d ./code/parameter_flash.o ./code/perspective.d ./code/perspective.o ./code/pico_gy.d ./code/pico_gy.o ./code/pico_link_ii.d ./code/pico_link_ii.o ./code/point_judge.d ./code/point_judge.o

.PHONY: clean-code


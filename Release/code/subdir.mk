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

COMPILED_SRCS += \
./code/All_Init.src \
./code/Filter.src \
./code/PID.src \
./code/PIT_Demo.src \
./code/PTS_Deal.src \
./code/Ring.src \
./code/Show_data.src \
./code/clip.src \
./code/cross.src \
./code/image_processing.src \
./code/key.src \
./code/menu.src \
./code/menu_list.src \
./code/parameter_flash.src \
./code/perspective.src \
./code/pico_gy.src \
./code/pico_link_ii.src \
./code/point_judge.src 

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
code/%.src: ../code/%.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/智能车摄像头/代码/前车/Front_Car_V2.4.4/Release/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O2 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/code\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

code/%.o: ./code/%.src code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-code

clean-code:
	-$(RM) ./code/All_Init.d ./code/All_Init.o ./code/All_Init.src ./code/Filter.d ./code/Filter.o ./code/Filter.src ./code/PID.d ./code/PID.o ./code/PID.src ./code/PIT_Demo.d ./code/PIT_Demo.o ./code/PIT_Demo.src ./code/PTS_Deal.d ./code/PTS_Deal.o ./code/PTS_Deal.src ./code/Ring.d ./code/Ring.o ./code/Ring.src ./code/Show_data.d ./code/Show_data.o ./code/Show_data.src ./code/clip.d ./code/clip.o ./code/clip.src ./code/cross.d ./code/cross.o ./code/cross.src ./code/image_processing.d ./code/image_processing.o ./code/image_processing.src ./code/key.d ./code/key.o ./code/key.src ./code/menu.d ./code/menu.o ./code/menu.src ./code/menu_list.d ./code/menu_list.o ./code/menu_list.src ./code/parameter_flash.d ./code/parameter_flash.o ./code/parameter_flash.src ./code/perspective.d ./code/perspective.o ./code/perspective.src ./code/pico_gy.d ./code/pico_gy.o ./code/pico_gy.src ./code/pico_link_ii.d ./code/pico_link_ii.o ./code/pico_link_ii.src ./code/point_judge.d ./code/point_judge.o ./code/point_judge.src

.PHONY: clean-code


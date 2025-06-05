################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../code/All_Init.c" \
"../code/Filter.c" \
"../code/PID.c" \
"../code/PIT_Demo.c" \
"../code/PTS_Deal.c" \
"../code/Ring.c" \
"../code/Show_data.c" \
"../code/Zebra.c" \
"../code/buzzer.c" \
"../code/clip.c" \
"../code/cross.c" \
"../code/exposure_adjust.c" \
"../code/image_processing.c" \
"../code/image_storage.c" \
"../code/infrared_nec.c" \
"../code/key.c" \
"../code/ladrc.c" \
"../code/menu.c" \
"../code/menu_list.c" \
"../code/parameter_flash.c" \
"../code/perspective.c" \
"../code/pico_gy.c" \
"../code/pico_link_ii.c" \
"../code/point_judge.c" \
"../code/speed_decision.c" \
"../code/w25n04.c" 

COMPILED_SRCS += \
"code/All_Init.src" \
"code/Filter.src" \
"code/PID.src" \
"code/PIT_Demo.src" \
"code/PTS_Deal.src" \
"code/Ring.src" \
"code/Show_data.src" \
"code/Zebra.src" \
"code/buzzer.src" \
"code/clip.src" \
"code/cross.src" \
"code/exposure_adjust.src" \
"code/image_processing.src" \
"code/image_storage.src" \
"code/infrared_nec.src" \
"code/key.src" \
"code/ladrc.src" \
"code/menu.src" \
"code/menu_list.src" \
"code/parameter_flash.src" \
"code/perspective.src" \
"code/pico_gy.src" \
"code/pico_link_ii.src" \
"code/point_judge.src" \
"code/speed_decision.src" \
"code/w25n04.src" 

C_DEPS += \
"./code/All_Init.d" \
"./code/Filter.d" \
"./code/PID.d" \
"./code/PIT_Demo.d" \
"./code/PTS_Deal.d" \
"./code/Ring.d" \
"./code/Show_data.d" \
"./code/Zebra.d" \
"./code/buzzer.d" \
"./code/clip.d" \
"./code/cross.d" \
"./code/exposure_adjust.d" \
"./code/image_processing.d" \
"./code/image_storage.d" \
"./code/infrared_nec.d" \
"./code/key.d" \
"./code/ladrc.d" \
"./code/menu.d" \
"./code/menu_list.d" \
"./code/parameter_flash.d" \
"./code/perspective.d" \
"./code/pico_gy.d" \
"./code/pico_link_ii.d" \
"./code/point_judge.d" \
"./code/speed_decision.d" \
"./code/w25n04.d" 

OBJS += \
"code/All_Init.o" \
"code/Filter.o" \
"code/PID.o" \
"code/PIT_Demo.o" \
"code/PTS_Deal.o" \
"code/Ring.o" \
"code/Show_data.o" \
"code/Zebra.o" \
"code/buzzer.o" \
"code/clip.o" \
"code/cross.o" \
"code/exposure_adjust.o" \
"code/image_processing.o" \
"code/image_storage.o" \
"code/infrared_nec.o" \
"code/key.o" \
"code/ladrc.o" \
"code/menu.o" \
"code/menu_list.o" \
"code/parameter_flash.o" \
"code/perspective.o" \
"code/pico_gy.o" \
"code/pico_link_ii.o" \
"code/point_judge.o" \
"code/speed_decision.o" \
"code/w25n04.o" 


# Each subdirectory must supply rules for building sources it contributes
"code/All_Init.src":"../code/All_Init.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/All_Init.o":"code/All_Init.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Filter.src":"../code/Filter.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Filter.o":"code/Filter.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/PID.src":"../code/PID.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/PID.o":"code/PID.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/PIT_Demo.src":"../code/PIT_Demo.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/PIT_Demo.o":"code/PIT_Demo.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/PTS_Deal.src":"../code/PTS_Deal.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/PTS_Deal.o":"code/PTS_Deal.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Ring.src":"../code/Ring.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Ring.o":"code/Ring.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Show_data.src":"../code/Show_data.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Show_data.o":"code/Show_data.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Zebra.src":"../code/Zebra.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Zebra.o":"code/Zebra.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/buzzer.src":"../code/buzzer.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/buzzer.o":"code/buzzer.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/clip.src":"../code/clip.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/clip.o":"code/clip.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/cross.src":"../code/cross.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/cross.o":"code/cross.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/exposure_adjust.src":"../code/exposure_adjust.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/exposure_adjust.o":"code/exposure_adjust.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/image_processing.src":"../code/image_processing.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/image_processing.o":"code/image_processing.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/image_storage.src":"../code/image_storage.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/image_storage.o":"code/image_storage.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/infrared_nec.src":"../code/infrared_nec.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/infrared_nec.o":"code/infrared_nec.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/key.src":"../code/key.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/key.o":"code/key.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/ladrc.src":"../code/ladrc.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/ladrc.o":"code/ladrc.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/menu.src":"../code/menu.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/menu.o":"code/menu.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/menu_list.src":"../code/menu_list.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/menu_list.o":"code/menu_list.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/parameter_flash.src":"../code/parameter_flash.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/parameter_flash.o":"code/parameter_flash.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/perspective.src":"../code/perspective.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/perspective.o":"code/perspective.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/pico_gy.src":"../code/pico_gy.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/pico_gy.o":"code/pico_gy.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/pico_link_ii.src":"../code/pico_link_ii.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/pico_link_ii.o":"code/pico_link_ii.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/point_judge.src":"../code/point_judge.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/point_judge.o":"code/point_judge.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/speed_decision.src":"../code/speed_decision.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/speed_decision.o":"code/speed_decision.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/w25n04.src":"../code/w25n04.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fF:/Smart_car/code/前车/Front_Car_V2.6.1/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=0 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/w25n04.o":"code/w25n04.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-code

clean-code:
	-$(RM) ./code/All_Init.d ./code/All_Init.o ./code/All_Init.src ./code/Filter.d ./code/Filter.o ./code/Filter.src ./code/PID.d ./code/PID.o ./code/PID.src ./code/PIT_Demo.d ./code/PIT_Demo.o ./code/PIT_Demo.src ./code/PTS_Deal.d ./code/PTS_Deal.o ./code/PTS_Deal.src ./code/Ring.d ./code/Ring.o ./code/Ring.src ./code/Show_data.d ./code/Show_data.o ./code/Show_data.src ./code/Zebra.d ./code/Zebra.o ./code/Zebra.src ./code/buzzer.d ./code/buzzer.o ./code/buzzer.src ./code/clip.d ./code/clip.o ./code/clip.src ./code/cross.d ./code/cross.o ./code/cross.src ./code/exposure_adjust.d ./code/exposure_adjust.o ./code/exposure_adjust.src ./code/image_processing.d ./code/image_processing.o ./code/image_processing.src ./code/image_storage.d ./code/image_storage.o ./code/image_storage.src ./code/infrared_nec.d ./code/infrared_nec.o ./code/infrared_nec.src ./code/key.d ./code/key.o ./code/key.src ./code/ladrc.d ./code/ladrc.o ./code/ladrc.src ./code/menu.d ./code/menu.o ./code/menu.src ./code/menu_list.d ./code/menu_list.o ./code/menu_list.src ./code/parameter_flash.d ./code/parameter_flash.o ./code/parameter_flash.src ./code/perspective.d ./code/perspective.o ./code/perspective.src ./code/pico_gy.d ./code/pico_gy.o ./code/pico_gy.src ./code/pico_link_ii.d ./code/pico_link_ii.o ./code/pico_link_ii.src ./code/point_judge.d ./code/point_judge.o ./code/point_judge.src ./code/speed_decision.d ./code/speed_decision.o ./code/speed_decision.src ./code/w25n04.d ./code/w25n04.o ./code/w25n04.src

.PHONY: clean-code


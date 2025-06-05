################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerDcc.c \
../libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGhs.c \
../libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGnuc.c \
../libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerTasking.c 

C_DEPS += \
./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerDcc.d \
./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGhs.d \
./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGnuc.d \
./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerTasking.d 

OBJS += \
./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerDcc.o \
./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGhs.o \
./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGnuc.o \
./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerTasking.o 


# Each subdirectory must supply rules for building sources it contributes
libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/%.o: ../libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/%.c libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-gcc -std=c99 "@F:/智能车摄像头/代码/前车/Front_Car_V2.4.4/External GCC - Debug/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O0 -g3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc161 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-libraries-2f-infineon_libraries-2f-Infra-2f-Platform-2f-Tricore-2f-Compilers

clean-libraries-2f-infineon_libraries-2f-Infra-2f-Platform-2f-Tricore-2f-Compilers:
	-$(RM) ./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerDcc.d ./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerDcc.o ./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGhs.d ./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGhs.o ./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGnuc.d ./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerGnuc.o ./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerTasking.d ./libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/CompilerTasking.o

.PHONY: clean-libraries-2f-infineon_libraries-2f-Infra-2f-Platform-2f-Tricore-2f-Compilers


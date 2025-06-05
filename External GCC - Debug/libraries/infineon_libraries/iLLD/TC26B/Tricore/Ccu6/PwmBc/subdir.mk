################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/PwmBc/IfxCcu6_PwmBc.c 

C_DEPS += \
./libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/PwmBc/IfxCcu6_PwmBc.d 

OBJS += \
./libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/PwmBc/IfxCcu6_PwmBc.o 


# Each subdirectory must supply rules for building sources it contributes
libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/PwmBc/%.o: ../libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/PwmBc/%.c libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/PwmBc/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-gcc -std=c99 "@F:/智能车摄像头/代码/前车/Front_Car_V2.4.4/External GCC - Debug/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O0 -g3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc161 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-libraries-2f-infineon_libraries-2f-iLLD-2f-TC26B-2f-Tricore-2f-Ccu6-2f-PwmBc

clean-libraries-2f-infineon_libraries-2f-iLLD-2f-TC26B-2f-Tricore-2f-Ccu6-2f-PwmBc:
	-$(RM) ./libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/PwmBc/IfxCcu6_PwmBc.d ./libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/PwmBc/IfxCcu6_PwmBc.o

.PHONY: clean-libraries-2f-infineon_libraries-2f-iLLD-2f-TC26B-2f-Tricore-2f-Ccu6-2f-PwmBc


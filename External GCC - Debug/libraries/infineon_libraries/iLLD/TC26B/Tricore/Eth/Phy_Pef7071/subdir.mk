################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/infineon_libraries/iLLD/TC26B/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.c 

C_DEPS += \
./libraries/infineon_libraries/iLLD/TC26B/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.d 

OBJS += \
./libraries/infineon_libraries/iLLD/TC26B/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.o 


# Each subdirectory must supply rules for building sources it contributes
libraries/infineon_libraries/iLLD/TC26B/Tricore/Eth/Phy_Pef7071/%.o: ../libraries/infineon_libraries/iLLD/TC26B/Tricore/Eth/Phy_Pef7071/%.c libraries/infineon_libraries/iLLD/TC26B/Tricore/Eth/Phy_Pef7071/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-gcc -std=c99 "@F:/智能车摄像头/代码/前车/Front_Car_V2.4.4/External GCC - Debug/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O0 -g3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc161 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-libraries-2f-infineon_libraries-2f-iLLD-2f-TC26B-2f-Tricore-2f-Eth-2f-Phy_Pef7071

clean-libraries-2f-infineon_libraries-2f-iLLD-2f-TC26B-2f-Tricore-2f-Eth-2f-Phy_Pef7071:
	-$(RM) ./libraries/infineon_libraries/iLLD/TC26B/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.d ./libraries/infineon_libraries/iLLD/TC26B/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.o

.PHONY: clean-libraries-2f-infineon_libraries-2f-iLLD-2f-TC26B-2f-Tricore-2f-Eth-2f-Phy_Pef7071


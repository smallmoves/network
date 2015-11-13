################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/XYBitSwarm.cpp \
../src/XYDataHandler.cpp \
../src/XYExecutor.cpp \
../src/XYNetwork.cpp \
../src/XYNetworkInterface.cpp \
../src/XYTCPSocketLayer.cpp \
../src/XYThreadManager.cpp 

OBJS += \
./src/XYBitSwarm.o \
./src/XYDataHandler.o \
./src/XYExecutor.o \
./src/XYNetwork.o \
./src/XYNetworkInterface.o \
./src/XYTCPSocketLayer.o \
./src/XYThreadManager.o 

CPP_DEPS += \
./src/XYBitSwarm.d \
./src/XYDataHandler.d \
./src/XYExecutor.d \
./src/XYNetwork.d \
./src/XYNetworkInterface.d \
./src/XYTCPSocketLayer.d \
./src/XYThreadManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



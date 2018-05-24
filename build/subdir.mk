################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Application.cpp \
../Codec.cpp \
../Codecs.cpp \
../DataBuffer.cpp \
../Events.cpp \
../FileDataBuffer.cpp \
../Fiv.cpp \
../Image.cpp \
../ImageDrawable.cpp \
../JpegCodec.cpp \
../Magic.cpp \
../MainWindow.cpp \
../MemoryDataBuffer.cpp \
../ThreadLocalStream.cpp \
../main.cpp 

OBJS += \
./Application.o \
./Codec.o \
./Codecs.o \
./DataBuffer.o \
./Events.o \
./FileDataBuffer.o \
./Fiv.o \
./Image.o \
./ImageDrawable.o \
./JpegCodec.o \
./Magic.o \
./MainWindow.o \
./MemoryDataBuffer.o \
./ThreadLocalStream.o \
./main.o 

CPP_DEPS += \
./Application.d \
./Codec.d \
./Codecs.d \
./DataBuffer.d \
./Events.d \
./FileDataBuffer.d \
./Fiv.d \
./Image.d \
./ImageDrawable.d \
./JpegCodec.d \
./Magic.d \
./MainWindow.d \
./MemoryDataBuffer.d \
./ThreadLocalStream.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	${CXX} $(shell pkg-config --cflags cairomm-1.0) $(shell pkg-config --cflags exiv2) $(shell pkg-config --cflags gtkmm-3.0) $(CXXFLAGS) -std=c++14 -D_FILE_OFFSET_BITS=64 -DGL_GLEXT_PROTOTYPES -O2 -g -Wall -Wextra -Werror -c -fmessage-length=0 -Wshadow -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



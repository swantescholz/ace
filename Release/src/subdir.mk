################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AceApplication.cpp \
../src/AceIpc.cpp \
../src/AceSourceView.cpp \
../src/AceTextfile.cpp \
../src/AceTimer.cpp \
../src/AceUtil.cpp \
../src/main.cpp 

OBJS += \
./src/AceApplication.o \
./src/AceIpc.o \
./src/AceSourceView.o \
./src/AceTextfile.o \
./src/AceTimer.o \
./src/AceUtil.o \
./src/main.o 

CPP_DEPS += \
./src/AceApplication.d \
./src/AceIpc.d \
./src/AceSourceView.d \
./src/AceTextfile.d \
./src/AceTimer.d \
./src/AceUtil.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DGSEAL_ENABLE -pthread -I/usr/include/gtkmm-3.0 -I/usr/include/gtksourceview-3.0 -I/usr/lib/gtkmm-3.0/include -I/usr/include/atkmm-1.6 -I/usr/include/giomm-2.4 -I/usr/lib/giomm-2.4/include -I/usr/include/pangomm-1.4 -I/usr/lib/pangomm-1.4/include -I/usr/include/gtk-3.0 -I/usr/include/cairomm-1.0 -I/usr/lib/cairomm-1.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gtk-3.0/unix-print -I/usr/include/gdkmm-3.0 -I/usr/lib/gdkmm-3.0/include -I/usr/include/atk-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng14 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



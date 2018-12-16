# cmake_minimum_required(VERSION <specify CMake version here>)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(output2 bank.cpp bank.h process_control.h)

add_executable(output main.cpp bank.cpp bank.h process_control.h)

add_executable(output3 RR.cpp PCB.h)
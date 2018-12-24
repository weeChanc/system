# cmake_minimum_required(VERSION <specify CMake version here>)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(output2 bank.cpp bank.h process.h)

add_executable(output process.cpp process.h memory.h SysRes.h pcb.h)
add_executable(output3 memory.cpp )
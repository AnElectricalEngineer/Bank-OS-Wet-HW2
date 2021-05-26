cmake_minimum_required(VERSION 3.18)
project(OS_Wet_HW2)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(OS_Wet_HW2
        bank.cpp atm.cpp atm.h account.cpp account.h main.cpp bank.h)

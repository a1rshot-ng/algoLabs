cmake_minimum_required(VERSION 3.17)
project(lab1 C)

set(CMAKE_C_STANDARD 11)
#add_library
#target_...
#link_libraries
add_executable(lab1
        lab1.c matrix.c matrix.h input.c input.h)

cmake_minimum_required(VERSION 3.16)
project(lab3 C)

set(CMAKE_C_STANDARD 99)

#add_definitions(-DVEC_SIZE=5)

add_library(list list.h list.c)
add_library(table table.h table.c)

add_executable(lab3 lab3.c input.h input.c)
target_link_libraries(lab3 table list)
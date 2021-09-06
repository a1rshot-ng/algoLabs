cmake_minimum_required(VERSION 3.17)
project(lab3c C)

set(CMAKE_C_STANDARD 99)

add_library(list list.h list.c)
add_library(table table.h table.c)
add_executable(lab3c lab3.c input.h input.c)
target_link_libraries(lab3c table list)
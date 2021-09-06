cmake_minimum_required(VERSION 3.16)
project(metro C)

set(CMAKE_C_STANDARD 99)

add_library(list list.h ../list.c)
add_library(table table.h ../table.c)
add_library(graph graph.h ../graph.c)
add_executable(metro metro.c input.h ../input.c)

target_link_libraries(metro graph table list)

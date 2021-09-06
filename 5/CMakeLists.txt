cmake_minimum_required(VERSION 3.16)
project(lab5 C)

set(CMAKE_C_STANDARD 99)

add_library(list list.h list.c)
add_library(table table.h table.c)
add_library(graph graph.h graph.c)
add_executable(lab5 lab5.c input.h input.c climsg.h)

add_compile_definitions(lab5 PRIVATE VIZ_PATH="../viz.py")
target_link_libraries(lab5 graph table list)
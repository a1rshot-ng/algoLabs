cmake_minimum_required(VERSION 3.17)
project(lab2 C)

set(CMAKE_C_STANDARD 99)

#add_definitions(-DUSE_LIST)
#add_definitions(-DMAX_VECTOR_LENGTH=5)
#add_definitions(-DMAX_LEN=5)

add_library(list list.h list.c)
add_library(vector vector.h vector.c)
add_library(stack stack.h stack.c)

add_executable(lab2 lab2.c)
target_link_libraries(lab2 stack list vector)
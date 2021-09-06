cmake_minimum_required(VERSION 3.17)
project(lab4a C)

set(CMAKE_C_STANDARD 99)

add_library(bintree bintree.h bintree.c)
add_executable(lab4a lab4a.c input.h input.c)

target_link_libraries(lab4a bintree)
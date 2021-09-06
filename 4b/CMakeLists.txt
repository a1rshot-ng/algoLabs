cmake_minimum_required(VERSION 3.17)
project(lab4b C)

set(CMAKE_C_STANDARD 99)

add_library(list list.h list.c)
add_library(kdtree kdtree.h kdtree.c)
add_executable(lab4b lab4b.c input.h input.c climsg.h)

target_compile_definitions(kdtree PRIVATE N=3)
target_compile_definitions(lab4b PRIVATE TMP_DIR="/tmp")
target_compile_definitions(lab4b PRIVATE VIZ_PATH="../viz.py")

target_link_libraries(lab4b kdtree list)

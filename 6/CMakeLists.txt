cmake_minimum_required(VERSION 3.16)
project(lab6)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread")

add_library(knapsack knapsack.cpp)
add_executable(lab6 lab6.cpp)

target_link_libraries(lab6 knapsack)
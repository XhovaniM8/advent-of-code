cmake_minimum_required(VERSION 3.20)
project(advent_of_code_2025 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Common header-only utilities (like input.hpp)
add_library(aoc_common INTERFACE)
target_include_directories(aoc_common INTERFACE ${CMAKE_SOURCE_DIR}/include)

# Day 1 (2025)
add_executable(aoc_2025_day01 2025/day01/main.cpp)
target_link_libraries(aoc_2025_day01 PRIVATE aoc_common)


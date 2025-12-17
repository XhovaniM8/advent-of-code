cmake_minimum_required(VERSION 3.20)
project(aoc_2025 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags for hardware-style optimization
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Enable testing
enable_testing()

# Find all day solutions
foreach(DAY RANGE 1 25)
    if(DAY LESS 10)
        set(DAY_PADDED "0${DAY}")
    else()
        set(DAY_PADDED "${DAY}")
    endif()
    
    set(DAY_DIR "${CMAKE_SOURCE_DIR}/2025/day${DAY_PADDED}")
    
    if(EXISTS "${DAY_DIR}/main.cpp")
        add_executable(aoc_2025_day${DAY_PADDED} ${DAY_DIR}/main.cpp)
        target_include_directories(aoc_2025_day${DAY_PADDED} PRIVATE ${CMAKE_SOURCE_DIR}/include)
        
        # Add test if expected output exists
        if(EXISTS "${CMAKE_SOURCE_DIR}/input/day${DAY_PADDED}.txt")
            add_test(
                NAME test_day${DAY_PADDED}
                COMMAND aoc_2025_day${DAY_PADDED} ${CMAKE_SOURCE_DIR}/input/day${DAY_PADDED}.txt
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            )
        endif()
    endif()
endforeach()

# Custom target for running all solutions
add_custom_target(run_all
    COMMENT "Running all AoC 2025 solutions"
)

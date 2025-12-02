# Advent of Code 2025 (C++)

Solutions for [Advent of Code 2025](https://adventofcode.com/2025), written in modern C++ (C++20).

## Layout

- `2025/day01/main.cpp` – Day 1 solution
- `include/input.hpp` – Shared utilities for reading input
- `input/day01.txt` – Puzzle inputs (not provided in this repo if AoC asks not to share)
- `CMakeLists.txt` – CMake build configuration
- `build/` – Out-of-source build directory (ignored)

## Build & Run

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
cd ..
./build/aoc_2025_day01


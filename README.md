# Advent of Code 2025 (C++ with Hardware Acceleration Focus)

Solutions for Advent of Code 2025, written in modern C++ (C++20) with a focus on modeling patterns suitable for FPGA/hardware acceleration.

## Hardware Acceleration Analysis

| Day | Problem | HW Potential | Pattern | Notes |
|-----|---------|:------------:|---------|-------|
| 3 | Lobby (battery joltage) | ⭐ | Parallel scan | Simple O(n) per row, min parallelism |
| 4 | Printing (neighbor count) | ⭐⭐⭐⭐ | 2D Stencil | Perfect for line buffers + sliding window |
| 5 | Cafeteria (range check) | ⭐⭐⭐ | Parallel comparators | All ranges checked simultaneously |
| 6 | Trash Compactor (math) | ⭐⭐ | Column-parallel MAC | Independent column operations |
| 7 | Laboratories (beam split) | ⭐⭐⭐⭐ | Cellular automata | Systolic array, wavefront propagation |
| 8 | Playground (3D distance) | ⭐⭐⭐ | Distance matrix | O(N²) parallel distance computation |
| 9 | Movie Theater (rectangle) | ⭐⭐ | Pair evaluation | Simple parallel comparisons |
| 10 | Factory (XOR lights) | ⭐⭐⭐⭐ | GF(2) algebra | Native XOR operations, bit manipulation |
| 11 | Reactor (path counting) | ⭐⭐ | DAG DP | Wavefront processing possible |
| 12 | Tree Farm (packing) | ⭐⭐⭐⭐⭐ | Constraint SAT | Massive parallel search, DLX friendly |

### Best Candidates for FPGA Implementation:
1. **Day 12** - Polyomino packing is a classic parallel backtracking problem
2. **Day 7** - Cellular automata maps directly to systolic arrays
3. **Day 4** - 2D stencil operations are a hardware classic
4. **Day 10** - XOR/bit operations are native to hardware

## Project Structure

```
advent-of-code/
├── 2025/
│   ├── day01/main.cpp    # Location Historian
│   ├── day02/main.cpp    # Red-Nosed Reports
│   ├── day03/main.cpp    # Lobby (battery joltage)
│   ├── day04/main.cpp    # Printing (neighbor counting)
│   ├── day05/main.cpp    # Cafeteria (range checking)
│   ├── day06/main.cpp    # Trash Compactor (column math)
│   ├── day07/main.cpp    # Laboratories (beam splitting)
│   ├── day08/main.cpp    # Playground (3D distance)
│   ├── day09/main.cpp    # Movie Theater (rectangles)
│   ├── day10/main.cpp    # Factory (XOR lights)
│   ├── day11/main.cpp    # Reactor (path counting)
│   └── day12/main.cpp    # Tree Farm (packing)
├── include/
│   └── aoc.hpp           # Shared utilities + HW modeling primitives
├── input/
│   └── dayXX.txt         # Puzzle inputs (not included)
├── tests/
│   └── test_solutions.py # Pytest-based test suite
├── .github/
│   └── workflows/
│       └── ci.yml        # GitHub Actions CI/CD
├── CMakeLists.txt
└── README.md
```

## Hardware Modeling Primitives

The `include/aoc.hpp` header provides hardware-modeling abstractions:

```cpp
// Register with clock enable
template<typename T> class Register;

// FIFO buffer (like BRAM FIFO)
template<typename T, size_t DEPTH> class FIFO;

// Parallel processing unit (simulates multiple PEs)
template<typename T, typename Func, size_t NUM_PES> class ParallelUnit;

// 2D Grid with hardware-friendly access (line buffer style)
template<typename T> class Grid2D;

// Union-Find for connectivity
class UnionFind;
```

## Build & Run

```bash
# Configure
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all solutions
cmake --build . -j$(nproc)

# Run a specific day
./aoc_2025_day04 ../input/day04.txt

# Run tests
ctest --output-on-failure
```

## Input Files

Place your puzzle inputs in `input/dayXX.txt` (e.g., `input/day04.txt`).

**Note:** Puzzle inputs are personalized and should not be shared per AoC guidelines.

## CI/CD Pipeline

GitHub Actions workflow includes:
- **Build & Test**: Compile all solutions, run CTest
- **Lint**: cppcheck for C++, flake8 for Python
- **Benchmark**: Performance timing on Release builds

## Requirements

- CMake 3.20+
- C++20 compiler (GCC 11+, Clang 14+)
- Python 3.8+ (for testing)
- pytest (for Python tests)

## License

MIT License

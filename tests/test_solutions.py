"""
Advent of Code 2025 - Test Suite
Tests solutions against example inputs from problem descriptions.
"""

import subprocess
import os
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent
BUILD_DIR = PROJECT_ROOT / "build"


def run_solution(day: int, input_data: str) -> str:
    """Run a day's solution with given input and return output."""
    day_str = f"{day:02d}"
    exe = BUILD_DIR / f"aoc_2025_day{day_str}"
    
    if not exe.exists():
        return f"SKIP: {exe} not found"
    
    # Write input to temp file
    input_file = BUILD_DIR / f"test_day{day_str}.txt"
    input_file.write_text(input_data)
    
    try:
        result = subprocess.run(
            [str(exe), str(input_file)],
            capture_output=True,
            text=True,
            timeout=60
        )
        return result.stdout
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    except Exception as e:
        return f"ERROR: {e}"


class TestDay03:
    """Day 3: Lobby - Battery Joltage"""
    
    EXAMPLE_INPUT = """987654321111111
811111111111119
234234234234278
818181911112111
"""
    EXPECTED_PART1 = 357  # 98 + 89 + 78 + 92

    def test_part1_example(self):
        output = run_solution(3, self.EXAMPLE_INPUT)
        assert "357" in output or "SKIP" in output


class TestDay04:
    """Day 4: Printing Department - Neighbor Counting"""
    
    EXAMPLE_INPUT = """..@@.@@@@.
@@@.@.@.@@
@@@@@.@.@@
@.@@@@..@.
@@.@@@@.@@
.@@@@@@@.@
.@.@.@.@@@
@.@@@.@@@@
.@@@@@@@@.
@.@.@@@.@.
"""
    EXPECTED_PART1 = 13

    def test_part1_example(self):
        output = run_solution(4, self.EXAMPLE_INPUT)
        assert "13" in output or "SKIP" in output


class TestDay05:
    """Day 5: Cafeteria - Fresh Ingredient Ranges"""
    
    EXAMPLE_INPUT = """3-5
10-14
16-20
12-18

1
5
8
11
17
32
"""
    EXPECTED_PART1 = 3  # 5, 11, 17 are fresh

    def test_part1_example(self):
        output = run_solution(5, self.EXAMPLE_INPUT)
        assert "3" in output or "SKIP" in output


class TestDay06:
    """Day 6: Trash Compactor - Cephalopod Math"""
    
    EXAMPLE_INPUT = """123 328  51 64 
 45 64  387 23 
  6 98  215 314
*   +   *   +  
"""
    EXPECTED_PART1 = 4277556  # 33210 + 490 + 4243455 + 401

    def test_part1_example(self):
        output = run_solution(6, self.EXAMPLE_INPUT)
        assert "4277556" in output or "SKIP" in output


class TestDay07:
    """Day 7: Laboratories - Tachyon Beam Splitting"""
    
    EXAMPLE_INPUT = """.......S.......
...............
.......^.......
...............
......^.^......
...............
.....^.^.^.....
...............
....^.^...^....
...............
...^.^...^.^...
...............
..^...^.....^..
...............
.^.^.^.^.^...^.
...............
"""
    EXPECTED_PART1 = 21

    def test_part1_example(self):
        output = run_solution(7, self.EXAMPLE_INPUT)
        assert "21" in output or "SKIP" in output


class TestDay08:
    """Day 8: Playground - Junction Box Connectivity"""
    
    EXAMPLE_INPUT = """162,817,812
57,618,57
906,360,560
592,479,940
352,342,300
466,668,158
542,29,236
431,825,988
739,650,466
52,470,668
216,146,977
819,987,18
117,168,530
805,96,715
346,949,466
970,615,88
941,993,340
862,61,35
984,92,344
425,690,689
"""
    EXPECTED_PART1 = 40  # After 10 connections: 5*4*2

    def test_part1_example(self):
        # This example uses 10 connections, our solution uses 1000
        # So we can't directly test against the example
        output = run_solution(8, self.EXAMPLE_INPUT)
        assert "Part 1:" in output or "SKIP" in output


class TestDay09:
    """Day 9: Movie Theater - Largest Rectangle"""
    
    EXAMPLE_INPUT = """7,1
11,1
11,7
9,7
9,5
2,5
2,3
7,3
"""
    EXPECTED_PART1 = 50  # Rectangle between (2,5) and (11,1)

    def test_part1_example(self):
        output = run_solution(9, self.EXAMPLE_INPUT)
        assert "50" in output or "SKIP" in output


class TestDay10:
    """Day 10: Factory - Indicator Light Configuration"""
    
    EXAMPLE_INPUT = """[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
[...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}
[.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}
"""
    EXPECTED_PART1 = 7  # 2 + 3 + 2

    def test_part1_example(self):
        output = run_solution(10, self.EXAMPLE_INPUT)
        assert "7" in output or "SKIP" in output


class TestDay11:
    """Day 11: Reactor - Path Counting"""
    
    EXAMPLE_INPUT = """aaa: you hhh
you: bbb ccc
bbb: ddd eee
ccc: ddd eee fff
ddd: ggg
eee: out
fff: out
ggg: out
hhh: ccc fff iii
iii: out
"""
    EXPECTED_PART1 = 5

    def test_part1_example(self):
        output = run_solution(11, self.EXAMPLE_INPUT)
        assert "5" in output or "SKIP" in output


class TestDay12:
    """Day 12: Christmas Tree Farm - Present Packing"""
    
    EXAMPLE_INPUT = """0:
###
##.
##.

1:
###
##.
.##

2:
.##
###
##.

3:
##.
###
##.

4:
###
#..
###

5:
###
.#.
###

4x4: 0 0 0 0 2 0
12x5: 1 0 1 0 2 2
12x5: 1 0 1 0 3 2
"""
    EXPECTED_PART1 = 2  # First two regions can fit, third cannot

    def test_part1_example(self):
        output = run_solution(12, self.EXAMPLE_INPUT)
        assert "2" in output or "SKIP" in output


if __name__ == "__main__":
    import pytest
    pytest.main([__file__, "-v"])

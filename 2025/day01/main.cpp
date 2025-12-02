#include "input.hpp"
#include <algorithm>
#include <iostream>

std::vector<std::string> example = {"L68", "L30", "R48", "L5",  "R60",
                                    "L55", "L1",  "L99", "R14", "L82"};

int count_zero_hits(const std::vector<std::string> &lines) {
  int pos = 50;
  int zero_hits = 0;

  for (const auto &line : lines) {
    if (line.empty())
      continue;

    char dir = line[0]; // 'L' or 'R'
    int dist = std::stoi(line.substr(1));

    if (dir == 'L') {
      pos = (pos - dist) % 100;
      if (pos < 0)
        pos += 100;
    } else if (dir == 'R') {
      pos = (pos + dist) % 100;
    } else {
      continue;
    }

    if (pos == 0) {
      ++zero_hits;
    }
  }
  return zero_hits;
}

int main() {
  try {
    // Read the file line-by-line
    auto lines = aoc::read_lines("input/day01.txt");

    // Now this is correct
    std::cout << count_zero_hits(lines) << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }

  // Optional test case
  // std::cout << count_zero_hits(example) << std::endl;

  return 0;
}

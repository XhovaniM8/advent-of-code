#include "input.hpp"
#include <iostream>
#include <string>
#include <vector>

// Part 1: only count when we *land* on 0 after a full rotation.
int count_zero_hits(const std::vector<std::string> &lines) {
  int pos = 50;
  int zero_hits = 0;

  for (const auto &line : lines) {
    if (line.empty())
      continue;

    char dir = line[0];
    int dist = std::stoi(line.substr(1));

    if (dir == 'L') {
      pos = (pos - dist) % 100;
      if (pos < 0)
        pos += 100;
    } else if (dir == 'R') {
      pos = (pos + dist) % 100;
    }

    if (pos == 0) {
      ++zero_hits;
    }
  }

  return zero_hits;
}

// Part 2: count *every click* that hits 0, including mid-rotation.
long long count_zero_clicks(const std::vector<std::string> &lines) {
  int pos = 50;             // current dial position
  long long total_hits = 0; // total clicks landing on 0

  for (const auto &line : lines) {
    if (line.empty())
      continue;

    char dir = line[0];
    long long dist = std::stoll(line.substr(1)); // distance might be large

    if (dir == 'R') {
      long long k0 = (100 - pos) % 100;
      if (k0 == 0)
        k0 = 100; // if pos==0, first hit is after 100 clicks

      if (dist >= k0) {
        total_hits += 1 + (dist - k0) / 100;
      }

      pos = static_cast<int>((pos + dist) % 100);

    } else if (dir == 'L') {
      long long k0 = pos % 100;
      if (k0 == 0)
        k0 = 100;

      if (dist >= k0) {
        total_hits += 1 + (dist - k0) / 100;
      }

      long long new_pos = pos - dist;
      new_pos %= 100;
      if (new_pos < 0)
        new_pos += 100;
      pos = static_cast<int>(new_pos);
    }
  }

  return total_hits;
}

int main() {
  try {
    auto lines = aoc::read_lines("input/day01.txt");

    int part1 = count_zero_hits(lines);
    long long part2 = count_zero_clicks(lines);

    std::cout << part1 << "\n";
    std::cout << part2 << "\n";

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

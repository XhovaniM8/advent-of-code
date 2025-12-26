// Day 1: Secret Entrance - Dial Safe

#include "aoc.hpp"

struct DialSimulator {
  int position = 50; // 7-bit register (0-99)
  int64_t zero_end_count = 0;
  int64_t zero_cross_count = 0;

  // Process a single rotation, counting zero crossings
  void rotate(char direction, int distance) {
    int step = (direction == 'R') ? 1 : -1;

    // Step through each click (for Part 2 zero-crossing detection)
    // In hardware: would be a state machine stepping through
    for (int i = 0; i < distance; i++) {
      position = (position + step + 100) % 100;
      if (position == 0) {
        zero_cross_count++;
      }
    }

    // Check if we ended at 0 (for Part 1)
    if (position == 0) {
      zero_end_count++;
    }
  }
};

std::pair<int64_t, int64_t> solve(const std::vector<std::string> &lines) {
  DialSimulator dial;

  for (const auto &line : lines) {
    if (line.empty())
      continue;

    char direction = line[0];
    int distance = std::stoi(line.substr(1));

    dial.rotate(direction, distance);
  }

  return {dial.zero_end_count, dial.zero_cross_count};
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }

  auto lines = aoc::read_lines(argv[1]);

  {
    aoc::Timer t("Both Parts");
    auto [part1, part2] = solve(lines);
    std::cout << "Part 1: " << part1 << "\n";
    std::cout << "Part 2: " << part2 << "\n";
  }

  return 0;
}

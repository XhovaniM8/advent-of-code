// Day 3: Lobby - Battery Joltage
// Hardware Model: Streaming max finder with running state
//
// Find the maximum 2-digit joltage by selecting exactly 2 batteries.
// The joltage is formed by the digits in their original order.
// Need to find max over all pairs (i,j) where i < j of: digit[i]*10 + digit[j]
//
// FPGA Implementation Notes:
// - Single pass: track max digit seen so far
// - For each new digit d at position j: candidate = max_so_far * 10 + d
// - Update running maximum joltage
// - Fully pipelined, one result per row

#include "aoc.hpp"

int64_t solve_part1(const std::vector<std::string> &lines) {
  int64_t total = 0;

  for (const auto &line : lines) {
    if (line.empty())
      continue;

    int max_joltage = 0;
    int max_digit_so_far = 0; // Maximum digit seen to the left

    for (char c : line) {
      if (c >= '1' && c <= '9') {
        int digit = c - '0';

        // Best joltage using this digit as the second digit
        if (max_digit_so_far > 0) {
          int joltage = max_digit_so_far * 10 + digit;
          max_joltage = std::max(max_joltage, joltage);
        }

        // Update max digit seen for future iterations
        max_digit_so_far = std::max(max_digit_so_far, digit);
      }
    }

    total += max_joltage;
  }

  return total;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }

  auto lines = aoc::read_lines(argv[1]);

  {
    aoc::Timer t("Part 1");
    auto result = solve_part1(lines);
    std::cout << "Part 1: " << result << "\n";
  }

  return 0;
}

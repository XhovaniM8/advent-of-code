// Day 2: Gift Shop - Invalid Product IDs

#include "aoc.hpp"

// Check if a number is made of a repeated pattern
// Part 1: exactly 2 repetitions
// Part 2: 2 or more repetitions
bool is_repeated_pattern(int64_t num, bool allow_more_than_two) {
  std::string s = std::to_string(num);
  int len = s.length();

  // Try all possible pattern lengths
  for (int pattern_len = 1; pattern_len <= len / 2; pattern_len++) {
    if (len % pattern_len != 0)
      continue;

    int repetitions = len / pattern_len;

    // Part 1: exactly 2 repetitions
    // Part 2: 2 or more repetitions
    if (!allow_more_than_two && repetitions != 2)
      continue;
    if (allow_more_than_two && repetitions < 2)
      continue;

    std::string pattern = s.substr(0, pattern_len);
    bool matches = true;

    // Check if all segments match the pattern
    for (int i = 1; i < repetitions && matches; i++) {
      if (s.substr(i * pattern_len, pattern_len) != pattern) {
        matches = false;
      }
    }

    if (matches)
      return true;
  }

  return false;
}

// Generate all invalid IDs in a range and sum them
int64_t sum_invalid_in_range(int64_t start, int64_t end, bool part2) {
  int64_t sum = 0;

  // Optimization: only check numbers that could be repeated patterns
  // A repeated pattern has length 2, 4, 6, 8, ... digits
  // We generate candidates directly instead of iterating all numbers

  // For small ranges, just iterate
  if (end - start < 100000) {
    for (int64_t num = start; num <= end; num++) {
      if (is_repeated_pattern(num, part2)) {
        sum += num;
      }
    }
    return sum;
  }

  // For large ranges, generate candidates
  // Generate all repeated patterns and check if they're in range
  int start_digits = std::to_string(start).length();
  int end_digits = std::to_string(end).length();

  for (int total_len = start_digits; total_len <= end_digits; total_len++) {
    // Try all pattern lengths that divide total_len
    for (int pattern_len = 1; pattern_len <= total_len / 2; pattern_len++) {
      if (total_len % pattern_len != 0)
        continue;

      int reps = total_len / pattern_len;
      if (!part2 && reps != 2)
        continue;
      if (part2 && reps < 2)
        continue;

      // Generate all patterns of this length
      int64_t pattern_start =
          (pattern_len == 1) ? 1 : std::pow(10, pattern_len - 1);
      int64_t pattern_end = std::pow(10, pattern_len) - 1;

      for (int64_t pattern = pattern_start; pattern <= pattern_end; pattern++) {
        // Build the repeated number
        std::string pattern_str = std::to_string(pattern);
        std::string num_str;
        for (int r = 0; r < reps; r++) {
          num_str += pattern_str;
        }

        int64_t num = std::stoll(num_str);

        if (num >= start && num <= end) {
          sum += num;
        }
      }
    }
  }

  return sum;
}

std::pair<int64_t, int64_t> solve(const std::string &input) {
  int64_t sum_part1 = 0;
  int64_t sum_part2 = 0;

  // Parse ranges from input (comma-separated, each is "start-end")
  std::string clean_input;
  for (char c : input) {
    if (!std::isspace(c))
      clean_input += c;
  }

  size_t pos = 0;
  while (pos < clean_input.length()) {
    // Find the range
    size_t dash = clean_input.find('-', pos);
    if (dash == std::string::npos)
      break;

    size_t comma = clean_input.find(',', dash);
    if (comma == std::string::npos)
      comma = clean_input.length();

    std::string start_str = clean_input.substr(pos, dash - pos);
    std::string end_str = clean_input.substr(dash + 1, comma - dash - 1);

    if (!start_str.empty() && !end_str.empty()) {
      int64_t start = std::stoll(start_str);
      int64_t end = std::stoll(end_str);

      sum_part1 += sum_invalid_in_range(start, end, false);
      sum_part2 += sum_invalid_in_range(start, end, true);
    }

    pos = comma + 1;
  }

  return {sum_part1, sum_part2};
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }

  std::string input = aoc::read_file(argv[1]);

  {
    aoc::Timer t("Both Parts");
    auto [part1, part2] = solve(input);
    std::cout << "Part 1: " << part1 << "\n";
    std::cout << "Part 2: " << part2 << "\n";
  }

  return 0;
}

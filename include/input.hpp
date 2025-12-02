#pragma once
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace aoc {
inline std::string read_file(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + path);
  }
  std::ostringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

inline std::vector<std::string> read_lines(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + path);
  }
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }
  return lines;
}

inline std::vector<int> read_ints(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + path);
  }
  std::vector<int> values;
  int x;
  while (file >> x) {
    values.push_back(x);
  }
  return values;
}

} // namespace aoc

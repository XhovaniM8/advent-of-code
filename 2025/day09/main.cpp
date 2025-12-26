// Day 9: Movie Theater - Largest Rectangle with Red Tile Corners
// Hardware Model: Parallel Rectangle Area Computation
//
// Given N red tiles, compute all O(N²) pairs and find max area.
// Each pair defines opposite corners of a rectangle.
// Area INCLUDES the boundary tiles, so area = (|x2-x1|+1) * (|y2-y1|+1)

#include "aoc.hpp"

struct Tile {
  int64_t x, y;
};

std::vector<Tile> parse_tiles(const std::vector<std::string> &lines) {
  std::vector<Tile> tiles;

  for (const auto &line : lines) {
    if (line.empty())
      continue;

    auto comma_pos = line.find(',');
    if (comma_pos != std::string::npos) {
      Tile t;
      t.x = std::stoll(line.substr(0, comma_pos));
      t.y = std::stoll(line.substr(comma_pos + 1));
      tiles.push_back(t);
    }
  }

  return tiles;
}

int64_t solve_part1(const std::vector<std::string> &lines) {
  auto tiles = parse_tiles(lines);
  int n = tiles.size();

  if (n < 2)
    return 0;

  int64_t max_area = 0;

  // Evaluate all pairs
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      // Rectangle area with boundary tiles included
      int64_t width = std::abs(tiles[i].x - tiles[j].x) + 1;
      int64_t height = std::abs(tiles[i].y - tiles[j].y) + 1;
      int64_t area = width * height;
      max_area = std::max(max_area, area);
    }
  }

  return max_area;
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

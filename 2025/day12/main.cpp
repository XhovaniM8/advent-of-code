// Day 12: Christmas Tree Farm - Present Packing
// Key insight: Shapes can leave gaps - only # cells matter for collision
#include "aoc.hpp"
#include <chrono>
#include <climits>
#include <set>

struct Shape {
  int index;
  std::vector<std::pair<int, int>> cells;
  int width = 0, height = 0;

  std::vector<Shape> all_orientations() const {
    std::vector<Shape> orientations;
    std::set<std::vector<std::pair<int, int>>> seen;

    Shape current = *this;
    for (int flip = 0; flip < 2; flip++) {
      for (int rot = 0; rot < 4; rot++) {
        current.normalize();
        if (seen.find(current.cells) == seen.end()) {
          seen.insert(current.cells);
          orientations.push_back(current);
        }
        current = current.rotate90();
      }
      current = current.flip_h();
    }
    return orientations;
  }

  Shape rotate90() const {
    Shape rotated;
    rotated.index = index;
    for (const auto &[dx, dy] : cells) {
      rotated.cells.push_back({dy, -dx});
    }
    rotated.normalize();
    return rotated;
  }

  Shape flip_h() const {
    Shape flipped;
    flipped.index = index;
    for (const auto &[dx, dy] : cells) {
      flipped.cells.push_back({-dx, dy});
    }
    flipped.normalize();
    return flipped;
  }

  void normalize() {
    if (cells.empty())
      return;
    int min_x = INT_MAX, min_y = INT_MAX;
    for (const auto &[dx, dy] : cells) {
      min_x = std::min(min_x, dx);
      min_y = std::min(min_y, dy);
    }
    for (auto &[dx, dy] : cells) {
      dx -= min_x;
      dy -= min_y;
    }
    width = height = 0;
    for (const auto &[dx, dy] : cells) {
      width = std::max(width, dx + 1);
      height = std::max(height, dy + 1);
    }
    std::sort(cells.begin(), cells.end());
  }

  int cell_count() const { return cells.size(); }
};

struct Region {
  int width, height;
  std::vector<int> piece_counts;
  std::vector<std::vector<bool>> grid;

  bool can_place(const Shape &shape, int x, int y) const {
    for (const auto &[dx, dy] : shape.cells) {
      int nx = x + dx, ny = y + dy;
      if (nx < 0 || nx >= width || ny < 0 || ny >= height)
        return false;
      if (grid[ny][nx])
        return false;
    }
    return true;
  }

  void place(const Shape &shape, int x, int y) {
    for (const auto &[dx, dy] : shape.cells) {
      grid[y + dy][x + dx] = true;
    }
  }

  void unplace(const Shape &shape, int x, int y) {
    for (const auto &[dx, dy] : shape.cells) {
      grid[y + dy][x + dx] = false;
    }
  }
};

class Solver {
  std::vector<std::vector<Shape>> all_shapes;
  std::vector<int> shape_sizes;
  bool timeout = false;
  std::chrono::steady_clock::time_point start_time;
  static constexpr int TIMEOUT_MS = 1000; // 1 second per region

public:
  void add_shape(const Shape &s) {
    while (all_shapes.size() <= static_cast<size_t>(s.index)) {
      all_shapes.push_back({});
      shape_sizes.push_back(0);
    }
    all_shapes[s.index] = s.all_orientations();
    shape_sizes[s.index] = s.cell_count();
  }

  bool can_fit(Region &region) {
    // Quick check: total # cells must fit in grid
    int total_cells = 0;
    for (size_t i = 0; i < region.piece_counts.size() && i < shape_sizes.size();
         i++) {
      total_cells += region.piece_counts[i] * shape_sizes[i];
    }
    if (total_cells > region.width * region.height) {
      return false;
    }

    std::vector<std::pair<int, int>> pieces;
    for (size_t i = 0; i < region.piece_counts.size(); i++) {
      if (i >= shape_sizes.size())
        continue;
      for (int j = 0; j < region.piece_counts[i]; j++) {
        pieces.push_back({static_cast<int>(i), shape_sizes[i]});
      }
    }

    std::sort(pieces.begin(), pieces.end(),
              [](auto &a, auto &b) { return a.second > b.second; });

    std::vector<int> piece_list;
    for (auto &[idx, sz] : pieces)
      piece_list.push_back(idx);

    if (piece_list.empty())
      return true;

    region.grid.assign(region.height, std::vector<bool>(region.width, false));

    timeout = false;
    start_time = std::chrono::steady_clock::now();
    return solve(region, piece_list, 0);
  }

private:
  bool check_timeout() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time)
            .count();
    return elapsed > TIMEOUT_MS;
  }

  bool solve(Region &region, const std::vector<int> &pieces, size_t idx) {
    if (idx >= pieces.size())
      return true;

    // Check timeout periodically
    if ((idx % 3 == 0) && check_timeout()) {
      timeout = true;
      return false;
    }

    int shape_idx = pieces[idx];
    if (shape_idx >= static_cast<int>(all_shapes.size()))
      return false;

    for (const auto &orient : all_shapes[shape_idx]) {
      for (int y = 0; y <= region.height - orient.height; y++) {
        for (int x = 0; x <= region.width - orient.width; x++) {
          if (region.can_place(orient, x, y)) {
            region.place(orient, x, y);
            if (solve(region, pieces, idx + 1))
              return true;
            region.unplace(orient, x, y);
            if (timeout)
              return false;
          }
        }
      }
    }
    return false;
  }
};

std::string trim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  if (start == std::string::npos)
    return "";
  size_t end = s.find_last_not_of(" \t\r\n");
  return s.substr(start, end - start + 1);
}

std::pair<std::vector<Shape>, std::vector<Region>>
parse_input(const std::vector<std::string> &lines) {
  std::vector<Shape> shapes;
  std::vector<Region> regions;

  size_t i = 0;
  while (i < lines.size()) {
    std::string line = trim(lines[i]);

    if (!line.empty() && line.back() == ':') {
      std::string num_str = line.substr(0, line.size() - 1);
      bool is_number = !num_str.empty() &&
                       std::all_of(num_str.begin(), num_str.end(), ::isdigit);

      if (is_number) {
        Shape shape;
        shape.index = std::stoi(num_str);
        i++;

        int row = 0;
        while (i < lines.size()) {
          std::string pattern_line = lines[i];
          bool has_hash = pattern_line.find('#') != std::string::npos;
          bool has_dot = pattern_line.find('.') != std::string::npos;

          if (!has_hash && !has_dot)
            break;
          if (pattern_line.find(':') != std::string::npos)
            break;
          if (pattern_line.find('x') != std::string::npos &&
              std::isdigit(pattern_line[0]))
            break;

          for (size_t col = 0; col < pattern_line.size(); col++) {
            if (pattern_line[col] == '#') {
              shape.cells.push_back({static_cast<int>(col), row});
            }
          }
          row++;
          i++;
        }

        shape.normalize();
        if (!shape.cells.empty()) {
          shapes.push_back(shape);
        }
        continue;
      }
    }

    size_t x_pos = line.find('x');
    size_t colon_pos = line.find(':');
    if (x_pos != std::string::npos && colon_pos != std::string::npos &&
        x_pos < colon_pos) {
      std::string width_str = line.substr(0, x_pos);
      std::string height_str = line.substr(x_pos + 1, colon_pos - x_pos - 1);

      bool valid_dims =
          !width_str.empty() && !height_str.empty() &&
          std::all_of(width_str.begin(), width_str.end(), ::isdigit) &&
          std::all_of(height_str.begin(), height_str.end(), ::isdigit);

      if (valid_dims) {
        Region region;
        region.width = std::stoi(width_str);
        region.height = std::stoi(height_str);

        std::string counts_str = line.substr(colon_pos + 1);
        std::istringstream iss(counts_str);
        int count;
        while (iss >> count) {
          region.piece_counts.push_back(count);
        }

        if (!region.piece_counts.empty()) {
          regions.push_back(region);
        }
      }
    }

    i++;
  }

  return {shapes, regions};
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }

  auto lines = aoc::read_lines(argv[1]);
  auto [shapes, regions] = parse_input(lines);

  std::cerr << "Parsed " << shapes.size() << " shapes and " << regions.size()
            << " regions\n";

  Solver solver;
  for (const auto &shape : shapes) {
    solver.add_shape(shape);
  }

  int64_t count = 0;
  {
    aoc::Timer t("Part 1");
    for (size_t r = 0; r < regions.size(); r++) {
      if (solver.can_fit(regions[r])) {
        count++;
      }
    }
    std::cout << "Part 1: " << count << "\n";
  }

  return 0;
}

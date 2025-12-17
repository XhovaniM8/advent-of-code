// Day 9: Movie Theater - Largest Rectangle with Red Tile Corners
// Hardware Model: Parallel Rectangle Area Computation
//
// Given N red tiles, compute all O(N²) pairs and find max area.
// Each pair defines opposite corners of a rectangle.
//
// FPGA Implementation Notes:
// - All pairs can be evaluated in parallel
// - Area = |x2-x1| * |y2-y1| (simple multiplier)
// - Reduction tree for max
// - With N tiles: N²/2 comparisons, highly parallelizable

#include "aoc.hpp"

struct Tile {
    int64_t x, y;
    
    // Rectangle area with this tile and another as opposite corners
    int64_t rectangle_area(const Tile& other) const {
        return std::abs(x - other.x) * std::abs(y - other.y);
    }
};

std::vector<Tile> parse_tiles(const std::vector<std::string>& lines) {
    std::vector<Tile> tiles;
    
    for (const auto& line : lines) {
        if (line.empty()) continue;
        
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

int64_t solve_part1(const std::vector<std::string>& lines) {
    auto tiles = parse_tiles(lines);
    int n = tiles.size();
    
    if (n < 2) return 0;
    
    int64_t max_area = 0;
    
    // Evaluate all pairs - in HW, parallel comparators + reduction
    // O(N²) pairs, but each is independent
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int64_t area = tiles[i].rectangle_area(tiles[j]);
            max_area = std::max(max_area, area);
        }
    }
    
    return max_area;
}

int main(int argc, char* argv[]) {
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

// Day 7: Laboratories - Tachyon Beam Splitting
// Hardware Model: Cellular Automata / Systolic Array
//
// THIS IS EXCELLENT FOR HARDWARE ACCELERATION!
// - Grid-based simulation with local rules
// - Each cell updates based only on neighbors
// - Wavefront propagation pattern
//
// FPGA Implementation Notes:
// - Systolic array: each PE handles one column
// - Row-by-row processing (streaming)
// - Beam state propagates down with splits
// - Counter per PE for split events
// - Reduction tree for total count

#include "aoc.hpp"
#include <set>

// Represents beam state at a position
// In HW: each column would track active beams with a bitmask or counter
struct BeamState {
    std::set<int> active_columns;  // Which columns have active beams
    
    void add_beam(int col) {
        active_columns.insert(col);
    }
    
    bool has_beam(int col) const {
        return active_columns.count(col) > 0;
    }
    
    void clear() {
        active_columns.clear();
    }
};

struct TachyonManifold {
    std::vector<std::string> grid;
    int width;
    int height;
    int start_col;
    
    int64_t count_splits() const {
        int64_t split_count = 0;
        
        // Track which columns have active beams
        // In HW: would be a register per column
        std::set<int> current_beams;
        std::set<int> next_beams;
        
        // Find start position 'S'
        current_beams.insert(start_col);
        
        // Process row by row (systolic: one row per clock cycle)
        for (int row = 0; row < height; row++) {
            next_beams.clear();
            
            for (int col : current_beams) {
                if (col < 0 || col >= width) continue;
                
                char cell = grid[row][col];
                
                if (cell == '^') {
                    // Splitter: emit beams left and right
                    split_count++;
                    
                    if (col - 1 >= 0) next_beams.insert(col - 1);
                    if (col + 1 < width) next_beams.insert(col + 1);
                } else if (cell == '.' || cell == 'S' || cell == '|') {
                    // Empty space or beam: continue downward
                    next_beams.insert(col);
                }
            }
            
            current_beams = next_beams;
            
            // Early exit if no more beams
            if (current_beams.empty()) break;
        }
        
        return split_count;
    }
};

TachyonManifold parse_input(const std::vector<std::string>& lines) {
    TachyonManifold manifold;
    manifold.grid = lines;
    manifold.height = lines.size();
    manifold.width = lines.empty() ? 0 : lines[0].size();
    manifold.start_col = -1;
    
    // Find 'S' position
    for (int row = 0; row < manifold.height; row++) {
        for (int col = 0; col < manifold.width; col++) {
            if (lines[row][col] == 'S') {
                manifold.start_col = col;
                break;
            }
        }
        if (manifold.start_col >= 0) break;
    }
    
    return manifold;
}

int64_t solve_part1(const std::vector<std::string>& lines) {
    auto manifold = parse_input(lines);
    return manifold.count_splits();
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

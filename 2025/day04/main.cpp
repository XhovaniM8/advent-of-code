// Day 4: Printing Department - Neighbor Counting
// Hardware Model: 2D Stencil Operation (3x3 window)
//
// This is PERFECT for hardware acceleration!
// - Each cell can be computed independently
// - Classic 2D convolution/stencil pattern
// - Line buffer + sliding window architecture
//
// FPGA Implementation Notes:
// - Use line buffers (2 rows of BRAM) for streaming
// - 3x3 sliding window with 9 comparators per PE
// - Fully pipelined: 1 result per clock after initial latency
// - Could tile for parallel PEs processing different regions

#include "aoc.hpp"

// Simulates a line buffer for streaming 2D operations
template<size_t WIDTH>
class LineBuffer {
    std::array<char, WIDTH> line0{};
    std::array<char, WIDTH> line1{};
    std::array<char, WIDTH> line2{};
    
public:
    void shift_in_row(const std::string& row) {
        // Shift lines up (in HW: circular buffer pointer)
        line0 = line1;
        line1 = line2;
        for (size_t i = 0; i < WIDTH && i < row.size(); i++) {
            line2[i] = row[i];
        }
    }
    
    // Get 3x3 window centered at (x, 1) in the buffer
    std::array<std::array<char, 3>, 3> get_window(size_t x) const {
        std::array<std::array<char, 3>, 3> window{};
        for (int dy = 0; dy < 3; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = static_cast<int>(x) + dx;
                if (nx >= 0 && nx < static_cast<int>(WIDTH)) {
                    const auto& line = (dy == 0) ? line0 : (dy == 1) ? line1 : line2;
                    window[dy][dx + 1] = line[nx];
                } else {
                    window[dy][dx + 1] = '.';  // Out of bounds = empty
                }
            }
        }
        return window;
    }
};

// Count neighbors that are '@' - pure combinational logic
int count_paper_neighbors(const std::array<std::array<char, 3>, 3>& window) {
    int count = 0;
    for (int dy = 0; dy < 3; dy++) {
        for (int dx = 0; dx < 3; dx++) {
            if (dy == 1 && dx == 1) continue;  // Skip center
            if (window[dy][dx] == '@') count++;
        }
    }
    return count;
}

int64_t solve_part1(const std::vector<std::string>& lines) {
    if (lines.empty()) return 0;
    
    size_t height = lines.size();
    size_t width = lines[0].size();
    
    int64_t accessible_count = 0;
    
    // Process grid with 3-row sliding window
    // In hardware: streaming with line buffers
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            if (lines[y][x] != '@') continue;
            
            // Count neighbors in 3x3 window
            int neighbors = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dy == 0 && dx == 0) continue;
                    int ny = static_cast<int>(y) + dy;
                    int nx = static_cast<int>(x) + dx;
                    if (ny >= 0 && ny < static_cast<int>(height) &&
                        nx >= 0 && nx < static_cast<int>(width) &&
                        lines[ny][nx] == '@') {
                        neighbors++;
                    }
                }
            }
            
            // Accessible if fewer than 4 neighbors
            if (neighbors < 4) {
                accessible_count++;
            }
        }
    }
    
    return accessible_count;
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

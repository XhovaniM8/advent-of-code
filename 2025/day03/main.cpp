// Day 3: Lobby - Battery Joltage
// Hardware Model: Parallel max-pair finder per row
// 
// Each row is processed independently (embarrassingly parallel).
// Within each row, we find the two largest digits to form max joltage.
// 
// FPGA Implementation Notes:
// - Each row can be processed by a separate PE
// - Within a row: single-pass max-2 tracker (2 registers)
// - Reduction tree for final sum

#include "aoc.hpp"

struct MaxTracker {
    // Hardware registers for tracking top 2 digits
    int first_max = -1;   // Largest digit
    int first_pos = -1;   // Position of largest
    int second_max = -1;  // Second largest
    int second_pos = -1;  // Position of second largest
    
    // Single cycle update (combinational logic)
    void update(int digit, int pos) {
        if (digit > first_max) {
            second_max = first_max;
            second_pos = first_pos;
            first_max = digit;
            first_pos = pos;
        } else if (digit > second_max) {
            second_max = digit;
            second_pos = pos;
        }
    }
    
    // Form the two-digit number (largest digit first)
    int get_joltage() const {
        if (first_pos < second_pos) {
            return first_max * 10 + second_max;
        } else {
            return second_max * 10 + first_max;
        }
    }
};

int64_t solve_part1(const std::vector<std::string>& lines) {
    int64_t total = 0;
    
    // Each row processed in parallel (in HW, would be systolic array)
    for (const auto& line : lines) {
        if (line.empty()) continue;
        
        MaxTracker tracker;
        
        // Stream through digits - single pass
        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] >= '1' && line[i] <= '9') {
                tracker.update(line[i] - '0', static_cast<int>(i));
            }
        }
        
        total += tracker.get_joltage();
    }
    
    return total;
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

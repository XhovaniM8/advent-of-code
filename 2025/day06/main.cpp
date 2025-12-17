// Day 6: Trash Compactor - Cephalopod Math
// Hardware Model: Column-parallel MAC (Multiply-Accumulate) units
//
// Problems are arranged in columns, each column is independent.
// Within a column: accumulate numbers, then apply final operation.
//
// FPGA Implementation Notes:
// - Parse columns in parallel (column-major access)
// - Each column has its own accumulator register
// - Final operation (+ or *) applied at column end
// - Reduction tree for grand total

#include "aoc.hpp"

struct Problem {
    std::vector<int64_t> numbers;
    char operation;  // '+' or '*'
    
    int64_t solve() const {
        if (numbers.empty()) return 0;
        
        if (operation == '+') {
            int64_t sum = 0;
            for (int64_t n : numbers) sum += n;
            return sum;
        } else {  // '*'
            int64_t product = 1;
            for (int64_t n : numbers) product *= n;
            return product;
        }
    }
};

std::vector<Problem> parse_problems(const std::vector<std::string>& lines) {
    if (lines.empty()) return {};
    
    size_t max_width = 0;
    for (const auto& line : lines) {
        max_width = std::max(max_width, line.size());
    }
    
    std::vector<Problem> problems;
    
    // Find column boundaries (columns separated by full space columns)
    std::vector<std::pair<size_t, size_t>> column_ranges;  // start, end (exclusive)
    
    size_t col = 0;
    while (col < max_width) {
        // Skip separator columns (all spaces)
        bool all_space = true;
        for (const auto& line : lines) {
            if (col < line.size() && line[col] != ' ') {
                all_space = false;
                break;
            }
        }
        
        if (all_space) {
            col++;
            continue;
        }
        
        // Found start of a problem column
        size_t start = col;
        
        // Find end of this problem (next all-space column or end)
        while (col < max_width) {
            bool is_separator = true;
            for (const auto& line : lines) {
                if (col < line.size() && line[col] != ' ') {
                    is_separator = false;
                    break;
                }
            }
            if (is_separator) break;
            col++;
        }
        
        column_ranges.push_back({start, col});
    }
    
    // Extract each problem
    for (const auto& [start, end] : column_ranges) {
        Problem prob;
        prob.operation = '+';  // Default
        
        for (const auto& line : lines) {
            if (start >= line.size()) continue;
            
            // Extract substring for this column
            std::string col_str;
            for (size_t c = start; c < end && c < line.size(); c++) {
                col_str += line[c];
            }
            
            // Trim and parse
            size_t first = col_str.find_first_not_of(' ');
            if (first == std::string::npos) continue;
            size_t last = col_str.find_last_not_of(' ');
            std::string trimmed = col_str.substr(first, last - first + 1);
            
            if (trimmed.empty()) continue;
            
            // Check if operation symbol
            if (trimmed == "+" || trimmed == "*") {
                prob.operation = trimmed[0];
            } else {
                // Try to parse as number
                try {
                    int64_t num = std::stoll(trimmed);
                    prob.numbers.push_back(num);
                } catch (...) {
                    // Not a valid number, skip
                }
            }
        }
        
        if (!prob.numbers.empty()) {
            problems.push_back(prob);
        }
    }
    
    return problems;
}

int64_t solve_part1(const std::vector<std::string>& lines) {
    auto problems = parse_problems(lines);
    
    int64_t grand_total = 0;
    
    // Each problem solved in parallel (in HW)
    // Then reduction tree for sum
    for (const auto& prob : problems) {
        grand_total += prob.solve();
    }
    
    return grand_total;
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

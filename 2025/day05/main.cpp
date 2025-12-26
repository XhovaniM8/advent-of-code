// Day 5: Cafeteria - Fresh Ingredient Range Checking

#include "aoc.hpp"

struct Range {
    int64_t start;
    int64_t end;
    
    // Hardware: Two comparators + AND gate
    bool contains(int64_t id) const {
        return id >= start && id <= end;
    }
};

// Parallel range checker - simulates hardware comparator array
class ParallelRangeChecker {
    std::vector<Range> ranges;
    
public:
    void add_range(int64_t start, int64_t end) {
        ranges.push_back({start, end});
    }
    
    // In hardware: All comparisons happen in parallel
    // OR reduction tree combines results
    bool is_fresh(int64_t id) const {
        // Simulates parallel check + OR reduction
        for (const auto& r : ranges) {
            if (r.contains(id)) return true;
        }
        return false;
    }
    
    size_t range_count() const { return ranges.size(); }
};

std::pair<std::vector<Range>, std::vector<int64_t>> parse_input(const std::vector<std::string>& lines) {
    std::vector<Range> ranges;
    std::vector<int64_t> ingredients;
    
    bool parsing_ranges = true;
    
    for (const auto& line : lines) {
        if (line.empty()) {
            parsing_ranges = false;
            continue;
        }
        
        if (parsing_ranges) {
            // Parse range like "3-5"
            auto dash_pos = line.find('-');
            if (dash_pos != std::string::npos) {
                int64_t start = std::stoll(line.substr(0, dash_pos));
                int64_t end = std::stoll(line.substr(dash_pos + 1));
                ranges.push_back({start, end});
            }
        } else {
            // Parse ingredient ID
            ingredients.push_back(std::stoll(line));
        }
    }
    
    return {ranges, ingredients};
}

int64_t solve_part1(const std::vector<std::string>& lines) {
    auto [ranges, ingredients] = parse_input(lines);
    
    ParallelRangeChecker checker;
    for (const auto& r : ranges) {
        checker.add_range(r.start, r.end);
    }
    
    int64_t fresh_count = 0;
    
    // Stream ingredients through checker
    // In hardware: one ingredient per clock cycle
    for (int64_t id : ingredients) {
        if (checker.is_fresh(id)) {
            fresh_count++;
        }
    }
    
    return fresh_count;
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

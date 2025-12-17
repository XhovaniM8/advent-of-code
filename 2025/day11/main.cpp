// Day 11: Reactor - Path Counting in DAG
// Hardware Model: Dynamic Programming / Memoized Graph Traversal
//
// Count all paths from "you" to "out" in a directed graph.
// Since data only flows forward, this is a DAG.
//
// FPGA Implementation Notes:
// - Topological sort + DP propagation
// - Each node accumulates path counts from predecessors
// - Wavefront processing (nodes ready when all inputs processed)
// - Could use systolic array for regular graph structures

#include "aoc.hpp"

struct Graph {
    std::unordered_map<std::string, std::vector<std::string>> adj;
    std::unordered_set<std::string> nodes;
    
    void add_edge(const std::string& from, const std::string& to) {
        adj[from].push_back(to);
        nodes.insert(from);
        nodes.insert(to);
    }
    
    // Count paths using DFS with memoization
    // In HW: would be DP with topological ordering
    int64_t count_paths(const std::string& start, const std::string& end) {
        std::unordered_map<std::string, int64_t> memo;
        return dfs_count(start, end, memo);
    }
    
private:
    int64_t dfs_count(const std::string& node, const std::string& end,
                      std::unordered_map<std::string, int64_t>& memo) {
        if (node == end) return 1;
        
        auto it = memo.find(node);
        if (it != memo.end()) return it->second;
        
        int64_t total = 0;
        if (adj.count(node)) {
            for (const auto& next : adj[node]) {
                total += dfs_count(next, end, memo);
            }
        }
        
        memo[node] = total;
        return total;
    }
};

Graph parse_graph(const std::vector<std::string>& lines) {
    Graph g;
    
    for (const auto& line : lines) {
        if (line.empty()) continue;
        
        // Parse "aaa: bbb ccc ddd"
        auto colon_pos = line.find(':');
        if (colon_pos == std::string::npos) continue;
        
        std::string from = line.substr(0, colon_pos);
        std::string rest = line.substr(colon_pos + 1);
        
        // Trim from
        while (!from.empty() && from.front() == ' ') from.erase(0, 1);
        while (!from.empty() && from.back() == ' ') from.pop_back();
        
        // Parse destinations
        std::istringstream iss(rest);
        std::string to;
        while (iss >> to) {
            g.add_edge(from, to);
        }
    }
    
    return g;
}

int64_t solve_part1(const std::vector<std::string>& lines) {
    Graph g = parse_graph(lines);
    return g.count_paths("you", "out");
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

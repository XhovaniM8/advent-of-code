#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cmath>
#include <functional>
#include <cassert>

namespace aoc {

// ============================================================================
// INPUT UTILITIES
// ============================================================================

inline std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

inline std::vector<std::string> read_lines(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

inline std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(s);
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// ============================================================================
// HARDWARE MODELING PRIMITIVES
// ============================================================================

// Simulates a register with clock enable
template<typename T>
class Register {
public:
    T value{};
    T next_value{};
    
    void clock() { value = next_value; }
    void set(T v) { next_value = v; }
    T get() const { return value; }
};

// Simulates a FIFO buffer (like BRAM FIFO)
template<typename T, size_t DEPTH>
class FIFO {
    std::array<T, DEPTH> buffer{};
    size_t read_ptr = 0;
    size_t write_ptr = 0;
    size_t count = 0;
    
public:
    bool push(const T& val) {
        if (count >= DEPTH) return false;
        buffer[write_ptr] = val;
        write_ptr = (write_ptr + 1) % DEPTH;
        count++;
        return true;
    }
    
    bool pop(T& val) {
        if (count == 0) return false;
        val = buffer[read_ptr];
        read_ptr = (read_ptr + 1) % DEPTH;
        count--;
        return true;
    }
    
    bool empty() const { return count == 0; }
    bool full() const { return count >= DEPTH; }
    size_t size() const { return count; }
};

// Parallel processing unit - simulates multiple PEs
template<typename T, typename Func, size_t NUM_PES>
class ParallelUnit {
    std::array<T, NUM_PES> pe_results{};
    
public:
    template<typename InputIt>
    void process(InputIt begin, InputIt end, Func f) {
        size_t n = std::distance(begin, end);
        size_t chunk_size = (n + NUM_PES - 1) / NUM_PES;
        
        // In hardware, all PEs would run in parallel
        // Here we simulate by processing chunks
        for (size_t pe = 0; pe < NUM_PES && pe * chunk_size < n; pe++) {
            size_t start = pe * chunk_size;
            size_t stop = std::min(start + chunk_size, n);
            pe_results[pe] = T{};
            for (size_t i = start; i < stop; i++) {
                pe_results[pe] = f(pe_results[pe], *(begin + i));
            }
        }
    }
    
    T reduce(std::function<T(T, T)> reducer, T init = T{}) {
        T result = init;
        for (const auto& r : pe_results) {
            result = reducer(result, r);
        }
        return result;
    }
};

// 2D Grid with hardware-friendly access patterns
template<typename T>
class Grid2D {
    std::vector<T> data;
    size_t width_, height_;
    
public:
    Grid2D() : width_(0), height_(0) {}
    Grid2D(size_t w, size_t h, T init = T{}) 
        : data(w * h, init), width_(w), height_(h) {}
    
    void resize(size_t w, size_t h, T init = T{}) {
        width_ = w;
        height_ = h;
        data.assign(w * h, init);
    }
    
    T& at(size_t x, size_t y) { return data[y * width_ + x]; }
    const T& at(size_t x, size_t y) const { return data[y * width_ + x]; }
    
    T& operator()(size_t x, size_t y) { return at(x, y); }
    const T& operator()(size_t x, size_t y) const { return at(x, y); }
    
    size_t width() const { return width_; }
    size_t height() const { return height_; }
    
    bool valid(int x, int y) const {
        return x >= 0 && x < (int)width_ && y >= 0 && y < (int)height_;
    }
    
    // Hardware-friendly neighbor counting (8-direction)
    int count_neighbors(size_t x, size_t y, T target) const {
        int count = 0;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (valid(nx, ny) && at(nx, ny) == target) {
                    count++;
                }
            }
        }
        return count;
    }
};

// ============================================================================
// TIMING UTILITIES
// ============================================================================

class Timer {
    std::chrono::high_resolution_clock::time_point start_;
    std::string name_;
    
public:
    Timer(const std::string& name = "Timer") : name_(name) {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
        std::cout << name_ << ": " << duration.count() << " µs\n";
    }
    
    long long elapsed_us() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
    }
};

// ============================================================================
// UNION-FIND (for connectivity problems)
// ============================================================================

class UnionFind {
    std::vector<int> parent, rank_;
    
public:
    UnionFind(int n) : parent(n), rank_(n, 0) {
        std::iota(parent.begin(), parent.end(), 0);
    }
    
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }
    
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;
        
        if (rank_[px] < rank_[py]) std::swap(px, py);
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
        return true;
    }
    
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

} // namespace aoc

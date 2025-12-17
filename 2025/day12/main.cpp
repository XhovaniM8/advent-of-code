// Day 12: Christmas Tree Farm - Present Packing (Polyomino Constraint Satisfaction)
// Hardware Model: Parallel Backtracking Search / SAT Solver
//
// THIS HAS MASSIVE HARDWARE ACCELERATION POTENTIAL!
// - Constraint satisfaction problem (CSP)
// - Similar to pentomino/polyomino packing
// - Backtracking search with massive parallelism
//
// FPGA Implementation Notes:
// - Each search branch can be explored in parallel
// - Bit-vector representation for grid state
// - Hardware-friendly constraint checking
// - Could implement as parallel SAT solver
// - DLX (Dancing Links) algorithm is very hardware-friendly

#include "aoc.hpp"

struct Shape {
    int index;
    std::vector<std::pair<int, int>> cells;  // Relative (dx, dy) positions
    int width, height;
    
    // Generate all rotations and reflections
    std::vector<Shape> all_orientations() const {
        std::vector<Shape> orientations;
        
        // Current shape
        orientations.push_back(*this);
        
        // Rotations (90, 180, 270)
        Shape current = *this;
        for (int r = 0; r < 3; r++) {
            current = current.rotate90();
            if (!current.equals(orientations)) {
                orientations.push_back(current);
            }
        }
        
        // Flip and rotations
        Shape flipped = flip();
        if (!flipped.equals(orientations)) {
            orientations.push_back(flipped);
        }
        
        current = flipped;
        for (int r = 0; r < 3; r++) {
            current = current.rotate90();
            if (!current.equals(orientations)) {
                orientations.push_back(current);
            }
        }
        
        return orientations;
    }
    
    Shape rotate90() const {
        Shape rotated;
        rotated.index = index;
        for (const auto& [dx, dy] : cells) {
            rotated.cells.push_back({-dy, dx});
        }
        rotated.normalize();
        return rotated;
    }
    
    Shape flip() const {
        Shape flipped;
        flipped.index = index;
        for (const auto& [dx, dy] : cells) {
            flipped.cells.push_back({-dx, dy});
        }
        flipped.normalize();
        return flipped;
    }
    
    void normalize() {
        if (cells.empty()) return;
        
        int min_x = cells[0].first, min_y = cells[0].second;
        for (const auto& [dx, dy] : cells) {
            min_x = std::min(min_x, dx);
            min_y = std::min(min_y, dy);
        }
        
        for (auto& [dx, dy] : cells) {
            dx -= min_x;
            dy -= min_y;
        }
        
        // Compute bounding box
        width = height = 0;
        for (const auto& [dx, dy] : cells) {
            width = std::max(width, dx + 1);
            height = std::max(height, dy + 1);
        }
        
        // Sort for consistent comparison
        std::sort(cells.begin(), cells.end());
    }
    
    bool equals(const std::vector<Shape>& others) const {
        for (const auto& other : others) {
            if (cells == other.cells) return true;
        }
        return false;
    }
};

struct Region {
    int width, height;
    std::vector<int> piece_counts;  // How many of each shape needed
    
    // Grid state as bit vector (1 = occupied)
    std::vector<std::vector<bool>> grid;
    
    bool can_place(const Shape& shape, int x, int y) const {
        for (const auto& [dx, dy] : shape.cells) {
            int nx = x + dx, ny = y + dy;
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) return false;
            if (grid[ny][nx]) return false;
        }
        return true;
    }
    
    void place(const Shape& shape, int x, int y) {
        for (const auto& [dx, dy] : shape.cells) {
            grid[y + dy][x + dx] = true;
        }
    }
    
    void remove(const Shape& shape, int x, int y) {
        for (const auto& [dx, dy] : shape.cells) {
            grid[y + dy][x + dx] = false;
        }
    }
};

class PuzzleSolver {
    std::vector<Shape> base_shapes;
    std::vector<std::vector<Shape>> all_orientations;  // Per shape
    
public:
    void add_shape(const Shape& s) {
        base_shapes.push_back(s);
        all_orientations.push_back(s.all_orientations());
    }
    
    bool can_fit(Region region) {
        // Collect all pieces needed
        std::vector<int> pieces_to_place;
        for (size_t i = 0; i < region.piece_counts.size(); i++) {
            for (int j = 0; j < region.piece_counts[i]; j++) {
                pieces_to_place.push_back(i);
            }
        }
        
        // Initialize grid
        region.grid.assign(region.height, std::vector<bool>(region.width, false));
        
        return backtrack(region, pieces_to_place, 0);
    }
    
private:
    bool backtrack(Region& region, const std::vector<int>& pieces, size_t idx) {
        if (idx >= pieces.size()) {
            return true;  // All pieces placed
        }
        
        int shape_idx = pieces[idx];
        if (shape_idx >= static_cast<int>(all_orientations.size())) {
            return false;
        }
        
        // Try all orientations and positions
        for (const auto& orientation : all_orientations[shape_idx]) {
            for (int y = 0; y <= region.height - orientation.height; y++) {
                for (int x = 0; x <= region.width - orientation.width; x++) {
                    if (region.can_place(orientation, x, y)) {
                        region.place(orientation, x, y);
                        
                        if (backtrack(region, pieces, idx + 1)) {
                            return true;
                        }
                        
                        region.remove(orientation, x, y);
                    }
                }
            }
        }
        
        return false;
    }
};

std::pair<std::vector<Shape>, std::vector<Region>> parse_input(const std::vector<std::string>& lines) {
    std::vector<Shape> shapes;
    std::vector<Region> regions;
    
    size_t i = 0;
    
    // Parse shapes
    while (i < lines.size()) {
        const auto& line = lines[i];
        
        // Check if this is a shape definition "N:"
        if (line.size() >= 2 && line[1] == ':' && std::isdigit(line[0])) {
            Shape shape;
            shape.index = line[0] - '0';
            i++;
            
            int row = 0;
            while (i < lines.size() && !lines[i].empty() && 
                   (lines[i].find('#') != std::string::npos || lines[i].find('.') != std::string::npos) &&
                   lines[i].find(':') == std::string::npos &&
                   lines[i].find('x') == std::string::npos) {
                for (size_t col = 0; col < lines[i].size(); col++) {
                    if (lines[i][col] == '#') {
                        shape.cells.push_back({static_cast<int>(col), row});
                    }
                }
                row++;
                i++;
            }
            
            shape.normalize();
            shapes.push_back(shape);
        }
        // Check if this is a region definition "WxH: ..."
        else if (line.find('x') != std::string::npos && line.find(':') != std::string::npos) {
            Region region;
            
            auto colon_pos = line.find(':');
            std::string dims = line.substr(0, colon_pos);
            std::string counts_str = line.substr(colon_pos + 1);
            
            auto x_pos = dims.find('x');
            region.width = std::stoi(dims.substr(0, x_pos));
            region.height = std::stoi(dims.substr(x_pos + 1));
            
            // Parse piece counts
            std::istringstream iss(counts_str);
            int count;
            while (iss >> count) {
                region.piece_counts.push_back(count);
            }
            
            regions.push_back(region);
            i++;
        }
        else {
            i++;
        }
    }
    
    return {shapes, regions};
}

int64_t solve_part1(const std::vector<std::string>& lines) {
    auto [shapes, regions] = parse_input(lines);
    
    PuzzleSolver solver;
    for (const auto& shape : shapes) {
        solver.add_shape(shape);
    }
    
    int64_t count = 0;
    for (auto& region : regions) {
        if (solver.can_fit(region)) {
            count++;
        }
    }
    
    return count;
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

// Day 8: Playground - Junction Box Connectivity

#include "aoc.hpp"
#include <cmath>

struct Point3D {
  int64_t x, y, z;

  double distance_to(const Point3D &other) const {
    double dx = static_cast<double>(x - other.x);
    double dy = static_cast<double>(y - other.y);
    double dz = static_cast<double>(z - other.z);
    return std::sqrt(dx * dx + dy * dy + dz * dz);
  }
};

struct Edge {
  int from, to;
  double distance;

  bool operator<(const Edge &other) const { return distance < other.distance; }
};

std::vector<Point3D> parse_points(const std::vector<std::string> &lines) {
  std::vector<Point3D> points;

  for (const auto &line : lines) {
    if (line.empty())
      continue;

    auto parts = aoc::split(line, ',');
    if (parts.size() >= 3) {
      Point3D p;
      p.x = std::stoll(parts[0]);
      p.y = std::stoll(parts[1]);
      p.z = std::stoll(parts[2]);
      points.push_back(p);
    }
  }

  return points;
}

int64_t solve_part1(const std::vector<std::string> &lines) {
  auto points = parse_points(lines);
  int n = points.size();

  if (n < 2)
    return 0;

  // Phase 1: Compute all pairwise distances
  // In hardware: parallel distance units, O(n²/P) with P PEs
  std::vector<Edge> edges;
  edges.reserve(n * (n - 1) / 2);

  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      edges.push_back({i, j, points[i].distance_to(points[j])});
    }
  }

  // Phase 2: Sort edges by distance
  // In hardware: streaming merge sort or bitonic sort network
  std::sort(edges.begin(), edges.end());

  // Phase 3: Process edges with Union-Find
  // Process 1000 shortest edges (connections), even if already in same circuit
  aoc::UnionFind uf(n);

  int edges_processed = 0;

  for (const auto &e : edges) {
    if (edges_processed >= 1000)
      break;

    // Always try to unite (may or may not change circuit structure)
    uf.unite(e.from, e.to);
    edges_processed++;
  }

  // Count circuit sizes
  std::unordered_map<int, int> circuit_sizes;
  for (int i = 0; i < n; i++) {
    circuit_sizes[uf.find(i)]++;
  }

  // Get three largest circuits
  std::vector<int> sizes;
  for (const auto &[root, size] : circuit_sizes) {
    sizes.push_back(size);
  }
  std::sort(sizes.rbegin(), sizes.rend());

  // Multiply top 3
  int64_t result = 1;
  for (int i = 0; i < 3 && i < static_cast<int>(sizes.size()); i++) {
    result *= sizes[i];
  }

  return result;
}

int main(int argc, char *argv[]) {
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

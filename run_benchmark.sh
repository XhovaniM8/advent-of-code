#!/bin/bash
# run_benchmark.sh - Build and benchmark all AoC 2025 solutions
# Collects metrics useful for FPGA implementation decisions

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p build

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}╔══════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║        AoC 2025 - Hardware Acceleration Analysis                 ║${NC}"
echo -e "${CYAN}╚══════════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Build all solutions with different optimization levels
echo -e "${BLUE}[1/4] Building solutions...${NC}"
for day in $(seq -w 1 12); do
  src="2025/day${day}/main.cpp"
  if [ -f "$src" ]; then
    echo -n "  Day $day: "
    g++ -std=c++20 -O3 -march=native -I include "$src" -o "build/day${day}" 2>/dev/null && echo -e "${GREEN}OK${NC}" || echo -e "${RED}FAIL${NC}"
  fi
done

echo ""
echo -e "${BLUE}[2/4] Analyzing input sizes...${NC}"
echo ""
printf "  %-6s %10s %10s %12s\n" "Day" "Lines" "Bytes" "Complexity"
printf "  %-6s %10s %10s %12s\n" "------" "----------" "----------" "------------"

declare -A input_lines
declare -A input_bytes

for day in $(seq -w 1 12); do
  input="input/day${day}.txt"
  if [ -f "$input" ]; then
    lines=$(wc -l <"$input")
    bytes=$(wc -c <"$input")
    input_lines[$day]=$lines
    input_bytes[$day]=$bytes

    # Estimate computational complexity
    if [ $lines -lt 100 ]; then
      complexity="Low"
    elif [ $lines -lt 1000 ]; then
      complexity="Medium"
    else
      complexity="High"
    fi

    printf "  %-6s %10d %10d %12s\n" "$day" "$lines" "$bytes" "$complexity"
  else
    printf "  %-6s %10s %10s %12s\n" "$day" "N/A" "N/A" "No input"
  fi
done

echo ""
echo -e "${BLUE}[3/4] Running benchmarks (3 iterations each)...${NC}"
echo ""

# Header for results
printf "  %-6s %12s %12s %12s %15s\n" "Day" "Min (µs)" "Avg (µs)" "Max (µs)" "Throughput"
printf "  %-6s %12s %12s %12s %15s\n" "------" "------------" "------------" "------------" "---------------"

declare -A min_times
declare -A avg_times
declare -A max_times

for day in $(seq -w 1 12); do
  exe="build/day${day}"
  input="input/day${day}.txt"

  if [ -f "$exe" ] && [ -f "$input" ]; then
    times=()
    outputs=""

    for i in 1 2 3; do
      # Run and capture both output and timing
      result=$("$exe" "$input" 2>&1)
      # Extract timing from output (assumes format "Part X: NNN µs")
      time_us=$(echo "$result" | grep -oP '\d+(?= µs)' | tail -1)
      if [ -n "$time_us" ]; then
        times+=($time_us)
      fi
      if [ $i -eq 1 ]; then
        outputs="$result"
      fi
    done

    if [ ${#times[@]} -gt 0 ]; then
      # Calculate min, avg, max
      min=${times[0]}
      max=${times[0]}
      sum=0
      for t in "${times[@]}"; do
        sum=$((sum + t))
        [ $t -lt $min ] && min=$t
        [ $t -gt $max ] && max=$t
      done
      avg=$((sum / ${#times[@]}))

      min_times[$day]=$min
      avg_times[$day]=$avg
      max_times[$day]=$max

      # Calculate throughput (input bytes / time)
      if [ -n "${input_bytes[$day]}" ] && [ $avg -gt 0 ]; then
        throughput=$(echo "scale=2; ${input_bytes[$day]} / $avg" | bc)
        throughput_str="${throughput} B/µs"
      else
        throughput_str="N/A"
      fi

      printf "  %-6s %12d %12d %12d %15s\n" "$day" "$min" "$avg" "$max" "$throughput_str"
    else
      printf "  %-6s %12s %12s %12s %15s\n" "$day" "ERROR" "ERROR" "ERROR" "N/A"
    fi
  else
    printf "  %-6s %12s %12s %12s %15s\n" "$day" "SKIP" "SKIP" "SKIP" "N/A"
  fi
done

echo ""
echo -e "${BLUE}[4/4] Hardware Implementation Analysis...${NC}"
echo ""

cat <<'ANALYSIS'
╔════════════════════════════════════════════════════════════════════════════╗
║                    FPGA IMPLEMENTATION RECOMMENDATIONS                      ║
╠════════════════════════════════════════════════════════════════════════════╣
║                                                                            ║
║  Day │ Pattern              │ HW Potential │ Key Resources                 ║
║  ────┼──────────────────────┼──────────────┼─────────────────────────────  ║
║   1  │ Modular counter      │ ⭐⭐         │ 7-bit counter, comparator     ║
║   2  │ Pattern matching     │ ⭐⭐         │ String comparators, divisor   ║
║   3  │ Streaming max        │ ⭐           │ 2 registers, comparator       ║
║   4  │ 2D Stencil           │ ⭐⭐⭐⭐⭐   │ Line buffers, 3x3 window      ║
║   5  │ Range checking       │ ⭐⭐⭐       │ Parallel comparator array     ║
║   6  │ Column-parallel MAC  │ ⭐⭐         │ MAC units, accumulators       ║
║   7  │ Cellular automata    │ ⭐⭐⭐⭐⭐   │ Systolic array, wavefront     ║
║   8  │ Distance matrix      │ ⭐⭐⭐       │ Parallel sqrt, sort network   ║
║   9  │ Pair evaluation      │ ⭐⭐         │ Parallel multipliers          ║
║  10  │ GF(2) XOR algebra    │ ⭐⭐⭐⭐     │ XOR gates, bit manipulation   ║
║  11  │ DAG path counting    │ ⭐⭐         │ DP registers, adder tree      ║
║  12  │ Constraint SAT       │ ⭐⭐⭐⭐⭐   │ Parallel search, DLX engine   ║
║                                                                            ║
╠════════════════════════════════════════════════════════════════════════════╣
║                         TOP RECOMMENDATIONS                                 ║
╠════════════════════════════════════════════════════════════════════════════╣
║                                                                            ║
║  🥇 Day 4 (2D Stencil) - CLASSIC FPGA PATTERN                              ║
║     • Line buffer architecture (2 rows BRAM)                               ║
║     • 3x3 sliding window with 8 comparators                                ║
║     • Fully pipelined: 1 pixel/cycle after latency                         ║
║     • Estimated: ~200 LUTs, 2 BRAM, 100+ MHz                               ║
║                                                                            ║
║  🥈 Day 7 (Beam Splitting) - SYSTOLIC ARRAY                                ║
║     • Column-parallel processing elements                                  ║
║     • Wavefront propagation (row by row)                                   ║
║     • State machine per column                                             ║
║     • Estimated: ~500 LUTs, 50+ MHz                                        ║
║                                                                            ║
║  🥉 Day 10 (XOR Lights) - NATIVE HARDWARE OPS                              ║
║     • Pure combinational XOR logic                                         ║
║     • Gaussian elimination over GF(2)                                      ║
║     • Bit-parallel operations                                              ║
║     • Estimated: ~300 LUTs, 200+ MHz                                       ║
║                                                                            ║
║  🏅 Day 12 (Packing) - MASSIVE PARALLELISM                                 ║
║     • Parallel backtracking search                                         ║
║     • Hardware SAT solver / DLX engine                                     ║
║     • Multiple search branches simultaneously                              ║
║     • Estimated: Complex, but huge speedup potential                       ║
║                                                                            ║
╚════════════════════════════════════════════════════════════════════════════╝
ANALYSIS

echo ""
echo -e "${GREEN}Benchmark complete!${NC}"
echo ""
echo "To run a specific day:"
echo "  ./build/dayXX input/dayXX.txt"
echo ""

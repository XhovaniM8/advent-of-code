#!/bin/bash
# run_all.sh - Build and run all AoC 2025 solutions
# Usage: ./run_all.sh [day_number]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p build

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

run_day() {
  local day=$(printf "%02d" $1)
  local src="2025/day${day}/main.cpp"
  local exe="build/day${day}"
  local input="input/day${day}.txt"

  if [ ! -f "$src" ]; then
    return
  fi

  echo -e "${CYAN}═══════════════════════════════════════${NC}"
  echo -e "${CYAN}  Day $day${NC}"
  echo -e "${CYAN}═══════════════════════════════════════${NC}"

  # Compile
  if ! g++ -std=c++20 -O3 -march=native -I include "$src" -o "$exe" 2>&1; then
    echo -e "${RED}Compilation failed${NC}"
    return
  fi

  # Run
  if [ -f "$input" ]; then
    timeout 120 "$exe" "$input" || echo -e "${RED}Runtime error or timeout${NC}"
  else
    echo -e "${YELLOW}No input file: $input${NC}"
  fi
  echo ""
}

if [ -n "$1" ]; then
  run_day "$1"
else
  for day in $(seq 1 12); do
    run_day "$day"
  done
fi

echo -e "${GREEN}Done!${NC}"

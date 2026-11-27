#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

make

bash src/part1/benchmark_part1.sh build/part1 results
bash src/part2/tas/benchmark_tas.sh build/part2/tas results
bash src/part2/tatas/benchmark_tatas.sh build/part2/tatas results
bash src/part2/benchmark_compare_lock.sh build/part2 results

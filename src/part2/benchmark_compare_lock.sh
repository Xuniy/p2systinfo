#faire un truc pour faire compare lock pour 1 2 4 6 8 16 32 pour tas et tatas


set -euo pipefail
export LC_ALL=C

BUILD_DIR="${1:-build/part2}"
OUTPUT_DIR="${2:-results}"
THREADS=(1 2 4 8 16 32)
REP=5

mkdir -p "$OUTPUT_DIR"

run_benchmark() {
    local type="$1"
    local exe="$BUILD_DIR/compare_lock"
    local csv="$OUTPUT_DIR/compare_lock_${type}.csv"

    if [ ! -x "$exe" ]; then
        echo "Exécutable introuvable : $exe" >&2
        exit 1
    fi

    echo "threads,rep,duration_sec" > "$csv"

    for n in "${THREADS[@]}"; do
        for ((rep = 1; rep <= REP; rep++)); do
            local start_ns end_ns elapsed_ns duration

            echo "compare_lock $type : $n threads, mesure $rep/$REP" >&2

            start_ns=$(date +%s%N)

            "$exe" "$n" "$type" >/dev/null

            end_ns=$(date +%s%N)
            elapsed_ns=$((end_ns - start_ns))

            duration=$(awk -v ns="$elapsed_ns" \
                'BEGIN { printf "%.6f", ns / 1000000000 }')

            echo "$n,$rep,$duration" >> "$csv"
        done
    done
}

run_benchmark "tas"
run_benchmark "tatas"

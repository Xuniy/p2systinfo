#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:-build/part2/tas}"
OUTPUT_DIR="${2:-output/part2/tas}"
THREADS=(2 4 8 16 32)
REP=5
mkdir -p "$OUTPUT_DIR"

run_benchmark() {
    local program="$1"
    local arguments="$2"
    local exe="$BUILD_DIR/$program"
    local csv="$OUTPUT_DIR/${program}.csv"
    if [ ! -x "$exe" ]; then
        echo "Le fichier source $exe n'existe pas ou n'est pas exécutable"
        exit 1
    fi

    
    echo "threads,rep,duration_sec" > "$csv"

    for i in "${THREADS[@]}"; do
        for j in $(seq 1 "$REP"); do
            local start_ns end_ns duration
            echo "$program : $i threads, mesure $j/$REP" >&2
            start_ns=$(date +%s%N)

            if [ "$arguments" = "one" ]; then
                "$exe" "$i" >/dev/null
            elif [ "$arguments" = "two" ]; then
                local split=$((i / 2))
                "$exe" "$split" "$split" >/dev/null
            else
                echo "Arguments invalides pour le programme $program" >&2
                exit 1
            fi

            end_ns=$(date +%s%N)
            duration=$(awk "BEGIN { printf \"%.6f\", ($end_ns - $start_ns) / 1000000000 }")
            echo "$i,$j,$duration" >> "$csv"
        done
    done 
}
run_benchmark "philosophes_tas" "one"
run_benchmark "producer_consumer_tas" "two"
run_benchmark "reader_writer_tas" "two"



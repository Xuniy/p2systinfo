#!/usr/bin/env bash
set -euo pipefail

SRC="$1"
CSV="$2"
THREADS="$3"
REP=5

if [ ! -x "$SRC" ]; then
    echo "Le fichier source $SRC n'existe pas ou n'est pas exécutable"
    exit 1
fi

mkdir -p "$(dirname "$CSV")"
echo "threads,duration_sec" > "$CSV"

for i in $(seq 1 "$THREADS"); do
    for j in $(seq 1 "$REP"); do
        echo "Utilisation de $i threads pour la rep $j"
        START=$(date +%s%N)
        "$SRC" "$i"
        END=$(date +%s%N)
        DURATION=$(awk "BEGIN { printf \"%.4f\", ($END - $START) / 1000000000 }")
        echo "$i,$DURATION" >> "$CSV"
    done
done 

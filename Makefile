CC := gcc
CFLAGS := -O0 -pthread -Wall

BUILD_DIR := build
RESULTS_DIR := results

# --- PARTIE 1 : Primitives POSIX ---
P1_SRC := src/part1
P1_BUILD := $(BUILD_DIR)/part1
P1_EXES := $(P1_BUILD)/philosophes \
           $(P1_BUILD)/producer_consumer \
           $(P1_BUILD)/reader_writer

# --- PARTIE 2 : Verrous par attente active (TATAS, TAS, compare_lock) ---
P2_TATAS_SRC := src/part2/tatas
P2_TATAS_BUILD := $(BUILD_DIR)/part2/tatas
P2_TATAS_EXES := $(P2_TATAS_BUILD)/philosophes_tatas \
                 $(P2_TATAS_BUILD)/producer_consumer_tatas \
                 $(P2_TATAS_BUILD)/reader_writer_tatas

P2_TAS_SRC := src/part2/tas
P2_TAS_BUILD := $(BUILD_DIR)/part2/tas
P2_TAS_EXES := $(P2_TAS_BUILD)/philosophes_tas \
               $(P2_TAS_BUILD)/producer_consumer_tas \
               $(P2_TAS_BUILD)/reader_writer_tas

P2_LOCK_SRC := src/part2
P2_LOCK_BUILD := $(BUILD_DIR)/part2
P2_LOCK_EXE := $(P2_LOCK_BUILD)/compare_lock

# Liste globale de tous les exécutables
ALL_EXES := $(P1_EXES) $(P2_TATAS_EXES) $(P2_TAS_EXES) $(P2_LOCK_EXE)

# ==============================================================================
# Cibles principales (make, make test, make clean)
# ==============================================================================
.PHONY: all part1 part2 lock test-lock test clean

# "make" compile tout (Partie 1 + Partie 2)
all: $(ALL_EXES)

# Permet aussi de compiler uniquement une partie si besoin : "make part1" ou "make part2"
part1: $(P1_EXES)
part2: $(P2_TATAS_EXES) $(P2_TAS_EXES) $(P2_LOCK_EXE)

lock: $(P2_LOCK_EXE)

test-lock: lock
	./src/part2/benchmark_compare_lock.sh "$(P2_LOCK_BUILD)" "$(RESULTS_DIR)"
	python3 graphes/generate_lock_plot.py "$(RESULTS_DIR)"

# "make test" compile tout puis lance les scripts de benchmark
test: all
	./src/part1/benchmark_part1.sh "$(P1_BUILD)" "$(RESULTS_DIR)"
	./src/part2/tatas/benchmark_tatas.sh "$(P2_TATAS_BUILD)" "$(RESULTS_DIR)"
	./src/part2/tas/benchmark_tas.sh "$(P2_TAS_BUILD)" "$(RESULTS_DIR)"
	./src/part2/benchmark_compare_lock.sh "$(P2_LOCK_BUILD)" "$(RESULTS_DIR)"

test-part1: part1
	./src/part1/benchmark_part1.sh "$(P1_BUILD)" "$(RESULTS_DIR)"
# "make clean" supprime tous les exécutables compilés et les fichiers générés
clean:
	rm -rf $(BUILD_DIR) $(RESULTS_DIR)

# ==============================================================================
# Règles de compilation par dossier
# ==============================================================================

# 1. Compilation Partie 1
$(P1_BUILD)/%: $(P1_SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

# 2. Compilation Partie 2 - TATAS (inclut le dossier tatas/ pour tatas.h et sem_tatas.h)
$(P2_TATAS_BUILD)/%: $(P2_TATAS_SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(P2_TATAS_SRC) $< -o $@

# 3. Compilation Partie 2 - TAS (inclut le dossier tas/ pour tas.h et sem_tas.h)
$(P2_TAS_BUILD)/%: $(P2_TAS_SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(P2_TAS_SRC) $< -o $@

# 4. Compilation Partie 2 - compare_lock (inclut tatas/, tas/ et btatas/ si utilisé)
$(P2_LOCK_BUILD)/compare_lock: $(P2_LOCK_SRC)/compare_lock.c $(P2_TAS_SRC)/tas.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(P2_TATAS_SRC) -I$(P2_TAS_SRC) -Isrc/part2/btatas $< -o $@

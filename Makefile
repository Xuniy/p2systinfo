CC = gcc
CFLAGS = -O0 -pthread -Wall

BUILD_DIR = build
PROGRAMS = philosophes producer_consumer reader_writer

PART1 = $(addprefix $(BUILD_DIR)/part1/,$(PROGRAMS))
TAS = $(addprefix $(BUILD_DIR)/part2/tas/,$(addsuffix _tas,$(PROGRAMS)))
TATAS = $(addprefix $(BUILD_DIR)/part2/tatas/,$(addsuffix _tatas,$(PROGRAMS)))
COMPARE = $(BUILD_DIR)/part2/compare_lock

.PHONY: all test clean lock

all: $(PART1) $(TAS) $(TATAS) $(COMPARE)

	$(BUILD_DIR)/part1/%: src/part1/%.c
		@mkdir -p $(@D)
		$(CC) $(CFLAGS) $< -o $@

	$(BUILD_DIR)/part2/tas/%: src/part2/tas/%.c src/part2/tas/tas.h src/part2/tas/sem_tas.h
		@mkdir -p $(@D)
		$(CC) $(CFLAGS) $< -o $@

	$(BUILD_DIR)/part2/tatas/%: src/part2/tatas/%.c src/part2/tatas/tatas.h src/part2/tatas/sem_tatas.h
		@mkdir -p $(@D)
		$(CC) $(CFLAGS) $< -o $@

	$(COMPARE): src/part2/compare_lock.c src/part2/tas/tas.h src/part2/tatas/tatas.h
		@mkdir -p $(@D)
		$(CC) $(CFLAGS) $< -o $@
		

lock: $(COMPARE)

test:
	bash experiments.sh

clean:
	rm -rf $(BUILD_DIR) results

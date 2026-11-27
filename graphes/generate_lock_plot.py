"""Graphique TAS : python3 graphes/generate_lock_plot.py [dossier_results]."""

import csv
from pathlib import Path
import statistics
import sys

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def main():
    results = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("results")
    durations = {n: [] for n in (1, 2, 4, 8, 16, 32)}
    with (results / "compare_lock_tas.csv").open(newline="") as source:
        for row in csv.DictReader(source):
            durations[int(row["threads"])].append(float(row["duration_sec"]))

    if any(len(values) != 5 for values in durations.values()):
        raise ValueError("Il faut cinq mesures pour chaque nombre de threads.")

    threads = list(durations)
    means = [statistics.mean(durations[n]) for n in threads]
    deviations = [statistics.stdev(durations[n]) for n in threads]

    fig, ax = plt.subplots()
    ax.errorbar(threads, means, yerr=deviations, fmt="o-", capsize=5,
                label="TAS : moyenne et ecart type (5 mesures)")
    ax.set_xscale("log", base=2)
    ax.set_xticks(threads, labels=threads)
    ax.set_ylim(bottom=0)
    ax.set_xlabel("Nombre de threads")
    ax.set_ylabel("Temps total d'execution (s)")
    ax.set_title("32768 sections critiques au total\n"
                 "10000 iterations par section, compilation -O0")
    ax.legend()
    fig.tight_layout()
    fig.savefig(results / "compare_lock.pdf")
    plt.close(fig)


if __name__ == "__main__":
    main()

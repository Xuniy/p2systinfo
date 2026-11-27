import os

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

RESULT_DIR = "results"
OUTPUT_DIR = "graphes"

def make_stats(csv_path):
    df = pd.read_csv(csv_path)
    stats_df = df.groupby("threads")["duration_sec"].agg(mean="mean", std="std").reset_index().sort_values("threads")
    return stats_df

stats = make_stats("results/philosophes.csv")

plt.figure(figsize=(8, 5))
plt.errorbar(
    stats["threads"],
    stats["mean"],
    yerr=stats["std"].fillna(0),
    marker="o",
    capsize=4,
    label="Philosophes (POSIX)",
)

plt.title("Temps d'exécution moyen — Philosophes")
plt.xlabel("Nombre de threads")
plt.ylabel("Temps moyen (s)")
plt.xticks([2, 4, 8, 16, 32])
plt.ylim(bottom=0)
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend()

os.makedirs(OUTPUT_DIR, exist_ok=True)
plt.savefig(os.path.join(OUTPUT_DIR, "test_philosophes.png"), dpi=300, bbox_inches="tight")
plt.close()
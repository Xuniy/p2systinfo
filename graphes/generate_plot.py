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
stats2=make_stats("results/reader_writer.csv")
stats3=make_stats("results/producer_consumer.csv")
plt.figure(figsize=(8, 5))
plt.errorbar(
    stats["threads"],
    stats["mean"],
    yerr=stats["std"].fillna(0),
    marker="o",
    capsize=4,
    label="Philosophes (POSIX)",
)

plt.errorbar(
    stats2["threads"],
    stats2["mean"],
    yerr=stats2["std"].fillna(0),
    marker="s",
    capsize=4,
    label="Reader_Writer (POSIX)",
)

plt.errorbar(
    stats3["threads"],
    stats3["mean"],
    yerr=stats3["std"].fillna(0),
    marker="^",
    capsize=4,
    label="Producer_Consumer (POSIX)",
)

plt.title("Temps d'exécution moyen")
plt.xlabel("Nombre de threads")
plt.ylabel("Temps moyen (s)")
plt.xticks([2, 4, 8, 16, 32])
plt.ylim(bottom=0)
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend()

os.makedirs(OUTPUT_DIR, exist_ok=True)
plt.savefig("Performances.png", dpi=300, bbox_inches="tight")
plt.close()
    

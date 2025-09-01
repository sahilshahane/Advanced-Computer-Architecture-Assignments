#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def plot_memory_metrics(files_with_labels, output_dir="plots_memory"):
    os.makedirs(output_dir, exist_ok=True)

    # Metrics to plot
    metrics = [
        "L1-dcache-load-misses",
        "l2_rqsts.miss",
        "LLC-load-misses",
        "cache-misses",
        "mem_load_retired.l3_miss",
        "cache-references"
    ]

    # Load all CSVs
    data = {}
    for file, label in files_with_labels:
        df = pd.read_csv(file)
        data[label] = df

    # Create separate plots for each metric
    for metric in metrics:
        plt.figure(figsize=(8, 6))
        for label, df in data.items():
            plt.plot(df["TableSize"], df[metric], marker="o", label=label)

        plt.xlabel("Table Size")
        plt.ylabel(metric)
        plt.title(f"{metric} vs Table Size")
        plt.legend()
        plt.grid(True, linestyle="--", alpha=0.6)
        plt.tight_layout()

        output_file = os.path.join(output_dir, f"{metric.replace('.', '_')}.png")
        plt.savefig(output_file)
        plt.close()
        print(f"Saved {output_file}")

def main():
    if len(sys.argv) < 4 or len(sys.argv[2:]) % 2 != 0:
        print("Usage: python plot_memory.py <output_dir> <file1.csv> <label1> [<file2.csv> <label2> ...]")
        sys.exit(1)

    output_dir = sys.argv[1]
    files_with_labels = [(sys.argv[i], sys.argv[i+1]) for i in range(2, len(sys.argv), 2)]

    plot_memory_metrics(files_with_labels, output_dir)

if __name__ == "__main__":
    main()

#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os
import re

def extract_register_size(filename: str) -> str:
    """Extract register size (number) from filename, e.g. simd_128_avg.csv -> 128"""
    match = re.search(r"simd_(\d+)", os.path.basename(filename))
    return match.group(1) if match else "Unknown"

def plot_metric(df, x_col, y_col, group_col, register_size, output_dir, label_map):
    plt.figure(figsize=(10, 6))

    for key, group in df.groupby(group_col):
        plt.plot(
            group[x_col],
            group[y_col],
            marker="o",
            label=f"{group_col} {key}"
        )

    # Custom labels
    x_label = "Embedding Dimension" if x_col == "InputSize" else x_col
    y_label = label_map.get(y_col, y_col.replace("_", " "))

    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(f"{y_label} vs {x_label} (per {group_col})")
    plt.suptitle(f"SIMD {register_size}", fontsize=14, fontweight="bold", y=1.02)
    plt.legend(title=group_col, bbox_to_anchor=(1.05, 1), loc="upper left")

    os.makedirs(output_dir, exist_ok=True)
    out_file = os.path.join(output_dir, f"{y_label.replace(' ', '_')}.png")
    plt.tight_layout()
    plt.savefig(out_file, dpi=300, bbox_inches="tight")
    plt.close()
    print(f"Saved: {out_file}")

def main(csv_file):
    df = pd.read_csv(csv_file)
    df = df.sort_values(by=["TableSize", "InputSize"])

    # Extract SIMD register size
    register_size = extract_register_size(csv_file)
    output_dir = f"graph_{register_size}"

    # Add derived metrics
    if "instructions" in df and "cycles" in df:
        df["IPC"] = df["instructions"] / df["cycles"]
    if "cache-references" in df and "cache-misses" in df:
        df["CacheMissRate"] = (df["cache-misses"] / df["cache-references"]) * 100

    # Add MPKI metrics
    if "instructions" in df:
        denom = df["instructions"] / 1000
        if "L1-dcache-load-misses" in df:
            df["L1 MPKI"] = df["L1-dcache-load-misses"] / denom
        if "l2_rqsts.miss" in df:
            df["L2 MPKI"] = df["l2_rqsts.miss"] / denom
        if "LLC-load-misses" in df:
            df["LLC MPKI"] = df["LLC-load-misses"] / denom

    # Metric rename map
    label_map = {
        "TimeElapsedMicroSeconds": "ExecutionTime (microseconds)",
        "L1 MPKI": "L1 MPKI",
        "L2 MPKI": "L2 MPKI",
        "LLC MPKI": "LLC MPKI"
    }

    # Metrics to plot
    metrics = [
        "cycles",
        "instructions",
        "IPC",
        "TimeElapsedMicroSeconds",
        "L1-dcache-load-misses",
        "l2_rqsts.miss",
        "LLC-load-misses",
        "mem_load_retired.l3_miss",
        "CacheMissRate",
        "L1 MPKI",
        "L2 MPKI",
        "LLC MPKI",
        "branch-instructions",
        "branch-misses",
        "context-switches",
        "sw_prefetch_access.any",
    ]

    for metric in metrics:
        if metric in df:
            plot_metric(df, "InputSize", metric, "TableSize", register_size, output_dir, label_map)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python plot_all_metrics.py <input.csv>")
        sys.exit(1)

    csv_file = sys.argv[1]
    main(csv_file)

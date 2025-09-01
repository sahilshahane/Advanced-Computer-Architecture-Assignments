#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import sys
import re
import os

def extract_register_size(filename: str) -> str:
    """Extract register size (number) from filename, e.g. simd_128_avg.csv -> 128"""
    match = re.search(r"simd_(\d+)", os.path.basename(filename))
    return match.group(1) if match else "Unknown"

def plot_l1_misses(csv_file, output_file="l1_cache_misses.png"):
    # Read CSV
    df = pd.read_csv(csv_file)

    # Ensure numeric sorting
    df = df.sort_values(by=["TableSize", "InputSize"])

    # Extract register size from filename
    register_size = extract_register_size(csv_file)

    # Plot
    plt.figure(figsize=(10, 6))

    for table_size, group in df.groupby("TableSize"):
        plt.plot(
            group["InputSize"],
            group["L1-dcache-load-misses"],
            marker="o",
            label=f"TableSize {table_size}"
        )

    # Labels and title
    plt.xlabel("Input Size")
    plt.ylabel("L1 Cache Misses")
    plt.title(f"L1 Cache Misses vs Input Size (per Table Size)")
    plt.suptitle(f"SIMD {register_size} Bit", fontsize=14, fontweight="bold", y=1.02)

    plt.legend(title="Table Size", bbox_to_anchor=(1.05, 1), loc="upper left")

    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches="tight")
    plt.close()
    print(f"Graph saved as {output_file}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python plot_l1_misses.py <input.csv> [output.png]")
        sys.exit(1)

    csv_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else "l1_cache_misses.png"
    plot_l1_misses(csv_file, output_file)

#!/usr/bin/env python3
import pandas as pd
import sys
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) < 5 or len(sys.argv[3:]) % 2 != 0:
        print("Usage: python plot_speedup.py <baseline.csv> <output.png> <file1.csv> <label1> [<file2.csv> <label2> ...]")
        sys.exit(1)

    baseline_file = sys.argv[1]
    output_file = sys.argv[2]
    compare_args = sys.argv[3:]

    # Read baseline file (comma separated)
    baseline = pd.read_csv(baseline_file, sep=",")
    merge_keys = ["TileSize", "MatrixSize"]

    plt.figure(figsize=(8, 5))

    # Iterate through file-label pairs
    for i in range(0, len(compare_args), 2):
        f = compare_args[i]
        label = compare_args[i+1]

        df = pd.read_csv(f, sep=",")  # <- comma separated

        # Merge with baseline on MatrixSize + TileSize
        merged = baseline.merge(df, on=merge_keys, suffixes=("_base", "_cmp"))

        # Compute speedup
        merged["Speedup"] = merged["TimeElapsedSeconds_base"] / merged["TimeElapsedSeconds_cmp"]

        # Average speedup across TileSizes
        avg_speedup = merged.groupby("MatrixSize")["Speedup"].mean().reset_index()

        # Plot with custom label
        plt.plot(avg_speedup["MatrixSize"], avg_speedup["Speedup"], marker="o", label=label)

    plt.xlabel("Matrix Size")
    plt.ylabel("Speedup (Baseline / Compared)")
    plt.title("Speedup Comparison")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

    # Save plot to file
    plt.savefig(output_file,  dpi=500)
    print(f"✅ Speedup plot saved as {output_file}")

if __name__ == "__main__":
    main()

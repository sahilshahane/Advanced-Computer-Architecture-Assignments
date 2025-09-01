#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def plot_avg_speedup(csv_file, output_folder):
    # Read CSV
    df = pd.read_csv(csv_file)

    # Filter out Basic (ijk) from speedup
    df = df[df["Algorithm Type"] != "Basic (ijk)"]

    # Ensure output folder exists
    os.makedirs(output_folder, exist_ok=True)

    # Compute average SpeedUp per Algorithm
    avg_speedup = df.groupby("Algorithm Type")["SpeedUp over Basic"].mean()

    # Plot
    plt.figure(figsize=(10, 6))
    bars = plt.bar(avg_speedup.index, avg_speedup, color='skyblue', edgecolor='black')

    plt.title("Average SpeedUp Over Basic Algorithm (excluding Basic)")
    plt.xlabel("Algorithm Type")
    plt.ylabel("Average SpeedUp")
    plt.grid(axis='y', linestyle='--', alpha=0.5)
    plt.xticks(rotation=45)

    # Add numeric value on top of bars
    for bar in bars:
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2, yval + 0.05, f"{yval:.2f}", ha='center', va='bottom')

    plt.tight_layout()
    output_file = os.path.join(output_folder, "avg_speedup_comparison.png")
    plt.savefig(output_file)
    print(f"Average speedup plot saved to {output_file}")
    plt.close()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python plot_avg_speedup.py <csv_file> <output_folder>")
        sys.exit(1)

    plot_avg_speedup(sys.argv[1], sys.argv[2])

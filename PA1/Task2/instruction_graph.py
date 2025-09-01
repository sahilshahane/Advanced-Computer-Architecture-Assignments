#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os
import numpy as np

def plot_combined(output_file, file_label_pairs):
    dfs = []
    labels = []

    # Load CSVs
    for i in range(0, len(file_label_pairs), 2):
        file_path = file_label_pairs[i]
        label = file_label_pairs[i + 1]

        df = pd.read_csv(file_path, sep=",")
        df = df.rename(columns=lambda x: x.strip())

        if "InputSize" not in df.columns or "instructions" not in df.columns:
            print(f"Error: {file_path} must contain 'InputSize' and 'instructions' columns")
            sys.exit(1)

        df = df.sort_values("InputSize")
        df = df[["InputSize", "instructions"]].set_index("InputSize")
        dfs.append(df.rename(columns={"instructions": label}))
        labels.append(label)

    # Merge on InputSize
    combined = pd.concat(dfs, axis=1)

    # Plot grouped bar chart
    plt.figure(figsize=(10, 6))
    x = np.arange(len(combined.index))  # Input sizes
    width = 0.8 / len(labels)  # Bar width

    for i, label in enumerate(labels):
        plt.bar(x + i * width, combined[label], width, label=label)

    # Formatting
    ax = plt.gca()
    ax.set_xticks(x + width * (len(labels) - 1) / 2)
    ax.set_xticklabels(combined.index.astype(str), rotation=45, ha="right")  # rotate labels
    ax.ticklabel_format(style="plain", axis="y")
    ax.yaxis.get_major_formatter().set_useOffset(False)

    plt.xlabel("Embedding Dimension")
    plt.ylabel("Instruction Count")
    plt.title("Instruction Count vs Embedding Dimension (Comparison)")
    plt.grid(axis="y", linestyle="--", alpha=0.6)
    plt.legend()

    # Add note below plot
    plt.figtext(0.5, -0.02, "Note: On X axis, entries are for different table sizes",
                ha="center", fontsize=9, style="italic")

    plt.tight_layout()  # prevent overlap

    # Save one combined chart
    plt.savefig(output_file, dpi=500, bbox_inches="tight")
    plt.close()
    print(f"Saved combined bar chart -> {output_file}")


if __name__ == "__main__":
    if len(sys.argv) < 5 or len(sys.argv[2:]) % 2 != 0:
        print("Usage: python plot_instructions_bar.py <output.png> <file1.csv> <label1> [<file2.csv> <label2> ...]")
        sys.exit(1)

    output_file = sys.argv[1]
    file_label_pairs = sys.argv[2:]
    plot_combined(output_file, file_label_pairs)

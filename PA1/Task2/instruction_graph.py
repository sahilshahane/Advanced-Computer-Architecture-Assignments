#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def plot_bar(file_path, label):
    df = pd.read_csv(file_path, sep=",")
    df = df.rename(columns=lambda x: x.strip())

    if "InputSize" not in df.columns or "instructions" not in df.columns:
        print(f"Error: {file_path} must contain 'InputSize' and 'instructions' columns")
        sys.exit(1)

    df = df.sort_values("InputSize")

    # Bar chart (without value labels)
    plt.figure(figsize=(8, 5))
    plt.bar(df["InputSize"].astype(str), df["instructions"], label=label)

    # Formatting
    ax = plt.gca()
    ax.ticklabel_format(style="plain", axis="y")
    ax.yaxis.get_major_formatter().set_useOffset(False)

    plt.xlabel("Embedding Dimension")
    plt.ylabel("Instruction Count")
    plt.title(f"Instruction Count vs Input Size ({label})")
    plt.grid(axis="y", linestyle="--", alpha=0.6)
    plt.legend()

    out_file = os.path.splitext(file_path)[0] + "_bar.png"
    plt.savefig(out_file, dpi=300, bbox_inches="tight")
    plt.close()
    print(f"Saved bar chart -> {out_file}")


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python plot_instructions_bar.py <dummy.png> <file1.csv> <label1> [<file2.csv> <label2> ...]")
        sys.exit(1)

    # Ignore the first arg (combined name, not used now)
    file_label_pairs = sys.argv[2:]

    for i in range(0, len(file_label_pairs), 2):
        file_path = file_label_pairs[i]
        label = file_label_pairs[i + 1]
        plot_bar(file_path, label)

#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

def generate_all_plots(csv_file, output_folder):
    # Read CSV
    df = pd.read_csv(csv_file)
    os.makedirs(output_folder, exist_ok=True)

    # --- 1. Execution Time Comparison (Grouped Bar Chart) ---
    plt.figure(figsize=(10,6))
    matrix_sizes = sorted(df["Matrix Size"].unique())
    algorithms = df["Algorithm Type"].unique()

    width = 0.15  # bar width
    x = range(len(algorithms))
    
    for i, size in enumerate(matrix_sizes):
        times = df[df["Matrix Size"]==size]["TimeMs"]
        plt.bar([p + width*i for p in x], times, width=width, label=f'Matrix Size {size}')
    
    plt.xticks([p + width*(len(matrix_sizes)/2-0.5) for p in x], algorithms, rotation=45)
    plt.ylabel("Execution Time (ms)")
    plt.title("Execution Time Comparison")
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(output_folder, "execution_time_comparison.png"))
    plt.close()

    # --- 2. Average Execution Time ---
    avg_time = df.groupby("Algorithm Type")["TimeMs"].mean()
    plt.figure(figsize=(10,6))
    bars = plt.bar(avg_time.index, avg_time, color='lightgreen', edgecolor='black')
    plt.title("Average Execution Time per Algorithm")
    plt.ylabel("Average Time (ms)")
    plt.xticks(rotation=45)
    for bar in bars:
        plt.text(bar.get_x()+bar.get_width()/2, bar.get_height()+1, f"{bar.get_height():.1f}", ha='center', va='bottom')
    plt.tight_layout()
    plt.savefig(os.path.join(output_folder, "avg_execution_time.png"))
    plt.close()

    # --- 3. Neural Network Training Time Comparison ---
    plt.figure(figsize=(10,6))
    for algo in algorithms:
        subset = df[df["Algorithm Type"]==algo]
        plt.plot(subset["Matrix Size"], subset["NN Time"], marker='o', label=algo)
    plt.title("Neural Network Training Time")
    plt.xlabel("Matrix Size")
    plt.ylabel("NN Time (ms)")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(output_folder, "nn_time_comparison.png"))
    plt.close()

    # --- 4. Total Loss Comparison ---
    plt.figure(figsize=(10,6))
    total_loss = df.groupby("Algorithm Type")["Total Loss"].mean()
    bars = plt.bar(total_loss.index, total_loss, color='salmon', edgecolor='black')
    plt.title("Average Total Loss per Algorithm")
    plt.ylabel("Total Loss")
    plt.xticks(rotation=45)
    for bar in bars:
        plt.text(bar.get_x()+bar.get_width()/2, bar.get_height()+50, f"{bar.get_height():.2f}", ha='center', va='bottom')
    plt.tight_layout()
    plt.savefig(os.path.join(output_folder, "total_loss_comparison.png"))
    plt.close()

    # --- 5. Speedup vs Matrix Size (Line Plot, ignoring Basic) ---
    plt.figure(figsize=(10,6))
    df_speedup = df[df["Algorithm Type"] != "Basic (ijk)"]
    for algo in df_speedup["Algorithm Type"].unique():
        subset = df_speedup[df_speedup["Algorithm Type"]==algo]
        plt.plot(subset["Matrix Size"], subset["SpeedUp over Basic"], marker='o', label=algo)
    plt.title("SpeedUp vs Matrix Size")
    plt.xlabel("Matrix Size")
    plt.ylabel("SpeedUp over Basic")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(output_folder, "speedup_vs_matrix_size.png"))
    plt.close()

    # --- 6. Execution Time vs Matrix Size (Line Plot) ---
    plt.figure(figsize=(10,6))
    for algo in algorithms:
        subset = df[df["Algorithm Type"]==algo]
        plt.plot(subset["Matrix Size"], subset["TimeMs"], marker='o', label=algo)
    plt.title("Execution Time vs Matrix Size")
    plt.xlabel("Matrix Size")
    plt.ylabel("Time (ms)")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(output_folder, "time_vs_matrix_size.png"))
    plt.close()

    # --- 7. NN Time vs Total Loss (Scatter Plot) ---
    plt.figure(figsize=(10,6))
    for algo in algorithms:
        subset = df[df["Algorithm Type"]==algo]
        plt.scatter(subset["NN Time"], subset["Total Loss"], label=algo)
    plt.title("NN Training Time vs Total Loss")
    plt.xlabel("NN Time (ms)")
    plt.ylabel("Total Loss")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(output_folder, "nn_time_vs_total_loss.png"))
    plt.close()

    # --- 8. Average Speedup Bar Chart (excluding Basic) ---
    df_speedup_avg = df[df["Algorithm Type"] != "Basic (ijk)"]
    avg_speedup = df_speedup_avg.groupby("Algorithm Type")["SpeedUp over Basic"].mean()
    plt.figure(figsize=(10,6))
    bars = plt.bar(avg_speedup.index, avg_speedup, color='skyblue', edgecolor='black')
    for bar in bars:
        yval = bar.get_height()
        plt.text(bar.get_x()+bar.get_width()/2, yval+0.05, f"{yval:.2f}", ha='center', va='bottom')
    plt.title("Average SpeedUp Over Basic Algorithm (excluding Basic)")
    plt.ylabel("Average SpeedUp")
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig(os.path.join(output_folder, "avg_speedup.png"))
    plt.close()

    print(f"All plots saved in folder: {output_folder}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python generate_all_plots.py <csv_file> <output_folder>")
        sys.exit(1)

    generate_all_plots(sys.argv[1], sys.argv[2])

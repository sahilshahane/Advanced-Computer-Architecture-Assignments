import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def plot_matrix_instructions(file_path, output_file="output.png"):
    # Read CSV (comma-separated)
    df = pd.read_csv(file_path, sep=",")

    # Extract only required columns
    result = df[["MatrixSize", "Instructions"]]
    result["Instructions"] = result["Instructions"] / 10000


    # Plot
    plt.figure(figsize=(8, 5))
    plt.plot(result["MatrixSize"], result["Instructions"], marker="o", linestyle="-")
    plt.xlabel("Matrix Size")
    plt.ylabel("Instructions")
    plt.title("Matrix Size vs Instructions")
    plt.grid(True)

    # Force Y-axis to show full numbers, not scientific notation
    plt.ticklabel_format(style='plain', axis='y')

    # Show more ticks on Y-axis
    plt.locator_params(axis="y", nbins=15)  # increase number of ticks

    plt.text(
        0.5, -0.15, 
        "Note: Instructions are divided by 10000", 
        ha="center", va="center", transform=plt.gca().transAxes, fontsize=9
    )

    # Save plot
    plt.savefig(output_file, dpi=300, bbox_inches="tight")
    print(f"Plot saved as {os.path.abspath(output_file)}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script.py <csv_file> [output_file]")
    else:
        file_path = sys.argv[1]
        output_file = sys.argv[2] if len(sys.argv) > 2 else "output.png"
        plot_matrix_instructions(file_path, output_file)

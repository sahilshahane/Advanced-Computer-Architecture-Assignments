import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def plot_matrix_instructions(files_with_labels, output_file="output.png"):
    plt.figure(figsize=(8, 5))

    for file_path, label in files_with_labels:
        # Read CSV (comma-separated)
        df = pd.read_csv(file_path, sep=",")

        # Extract only required columns
        result = df[["MatrixSize", "Instructions"]]
        result["Instructions"] = result["Instructions"] / 10000

        # Plot
        plt.plot(result["MatrixSize"], result["Instructions"], marker="o", linestyle="-", label=label)

    plt.xlabel("Matrix Size")
    plt.ylabel("Instructions (divided by 10000)")
    plt.title("Matrix Size vs Instructions")
    plt.grid(True)

    # Force Y-axis to show full numbers, not scientific notation
    plt.ticklabel_format(style='plain', axis='y')

    # Show more ticks on Y-axis
    plt.locator_params(axis="y", nbins=15)

    # Add legend for labels
    plt.legend()

    # Save plot
    plt.savefig(output_file, dpi=500, bbox_inches="tight")
    print(f"Plot saved as {os.path.abspath(output_file)}")


if __name__ == "__main__":

    file1, label1 = "reports_sahil/Task_1C/simd_128_average.csv", "128 Bit"
    file2, label2 = "reports_sahil/Task_1C/simd_256_average.csv", "256 Bit"
    file3, label3 = "reports_sahil/Task_1C/simd_512_average.csv", "512 Bit"
    file4, label4 = "reports/naive/naive_average.csv", "Naive"

    output_file = "reports_sahil/Task_1C/simd_MatrixSize_Instructions.png"

    files_with_labels = [(file1, label1), (file2, label2), (file3, label3), (file4, label4)]
    plot_matrix_instructions(files_with_labels, output_file)

import pandas as pd
import matplotlib.pyplot as plt
import sys
from matplotlib.ticker import MaxNLocator, FuncFormatter

def plot_speedup(file1, file2, output_image="speedup.png"):
    # Load CSV files
    df1 = pd.read_csv(file1)
    df2 = pd.read_csv(file2)

    # Check required columns
    required_cols = ["TableSize", "InputSize", "TimeElapsedMicroSeconds"]
    for col in required_cols:
        if col not in df1.columns or col not in df2.columns:
            print(f"Error: Missing column {col} in one of the CSV files.")
            return

    # Merge the two files on TableSize and InputSize
    merged = pd.merge(df1, df2, on=["TableSize", "InputSize"], suffixes=('_1', '_2'))

    # Calculate speedup
    merged['Speedup'] = merged['TimeElapsedMicroSeconds_1'] / merged['TimeElapsedMicroSeconds_2']

    # Plot lines for each TableSize
    plt.figure(figsize=(12,6))
    for table_size in sorted(merged['TableSize'].unique()):
        subset = merged[merged['TableSize'] == table_size].sort_values('InputSize')
        plt.plot(
            subset['InputSize'],    
            subset['Speedup'],
            marker='o',
            label=f"{table_size}"
        )
    
    plt.xlabel("Embedding Dimension")
    plt.ylabel("Speedup")
    plt.ticklabel_format(style='plain', axis='y')
    plt.title("SIMD 512 Bit Speedup comparison over Naive")
    plt.legend(title="TableSize")
    plt.grid(True)

    # Increase X-axis bins
    ax = plt.gca()
    ax.xaxis.set_major_locator(MaxNLocator(integer=True, prune='lower', nbins=6))  # set ~10 bins

    # Show plain numbers on Y-axis
    ax.yaxis.set_major_formatter(FuncFormatter(lambda y, _: f'{y:.2f}'))

    plt.tight_layout()
    
    # Save the plot to file only
    plt.savefig(output_image)
    plt.close()
    print(f"Speedup plot saved as {output_image}")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python plot_speedup.py <file1.csv> <file2.csv> <output.png>")
        sys.exit(1)

    file1 = sys.argv[1]
    file2 = sys.argv[2]
    output_image = sys.argv[3]

    plot_speedup(file1, file2, output_image)

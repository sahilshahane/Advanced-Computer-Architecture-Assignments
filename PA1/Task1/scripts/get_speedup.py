#!/usr/bin/env python3
import pandas as pd
import sys

def main():
    if len(sys.argv) != 3:
        print("Usage: python script.py <file1.csv> <file2.csv>")
        sys.exit(1)

    file1, file2 = sys.argv[1], sys.argv[2]

    # Read CSVs (change sep="," if comma-separated instead of tab)
    df1 = pd.read_csv(file1, sep=",")
    df2 = pd.read_csv(file2, sep=",")

    # Ensure both have the same structure
    if not all(df1.columns == df2.columns):
        print("❌ The two CSV files have different structures!")
        sys.exit(1)

    # # Merge on TileSize + MatrixSize to align rows
    merge_keys = ["TileSize", "MatrixSize"]
    merged = df1.merge(df2, on=merge_keys, suffixes=("_1", "_2"))

    # Compute speedup
    merged["Speedup"] = (merged["TimeElapsedSeconds_1"] / merged["TimeElapsedSeconds_2"]).round(3)

    # Select only MatrixSize & Speedup
    result = merged[["MatrixSize", "Speedup"]]

    # Print as list
    speedup_list = list(result.itertuples(index=False, name=None))
    print("✅ Speedup list (MatrixSize, Speedup):")
    print(speedup_list)

if __name__ == "__main__":
    main()

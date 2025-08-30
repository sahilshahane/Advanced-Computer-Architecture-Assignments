#!/usr/bin/env python3
import pandas as pd
import sys
import glob
import os
import numpy as np

def main():
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_folder> <output.tsv>")
        sys.exit(1)

    input_folder, output = sys.argv[1], sys.argv[2]

    # Find all TSV/CSV files in the folder
    csv_files = glob.glob(os.path.join(input_folder, "*.csv")) + glob.glob(os.path.join(input_folder, "*.tsv"))
    
    if len(csv_files) < 2:
        print("❌ Need at least 2 CSV/TSV files in the folder to average.")
        sys.exit(1)

    print(f"📂 Found {len(csv_files)} files in {input_folder}")

    # Load all files
    dfs = [pd.read_csv(f, sep="\t") for f in csv_files]

    # Ensure all have same structure
    base_cols = dfs[0].columns.tolist()
    for i, df in enumerate(dfs, 1):
        if df.columns.tolist() != base_cols:
            print(f"⚠️ Warning: File {csv_files[i-1]} has different columns!")

    # Merge step: align on TileSize + MatrixSize
    merge_keys = ["TileSize", "MatrixSize"]
    merged = dfs[0]
    for i, df in enumerate(dfs[1:], start=2):
        merged = merged.merge(df, on=merge_keys, suffixes=("", f"_{i}"))

    # Compute averages
    non_numeric = merge_keys
    numeric_cols = [c for c in base_cols if c not in non_numeric]

    averaged = merged[merge_keys].copy()
    for col in numeric_cols:
        col_variants = [c for c in merged.columns if c.startswith(col)]
        averaged[col] = merged[col_variants].mean(axis=1)

    # Apply rounding rules
    for col in averaged.columns:
        if col in ['TimeElapsedSeconds']: 
            continue                 # no rounding
        if col in ["Instructions", "Cycles"]:
            averaged[col] = averaged[col].round(0).astype(int)   # integer rounding
        elif col not in merge_keys:
            averaged[col] = averaged[col].round(2)              # 2 decimal rounding

    # Sort by MatrixSize, then TileSize (optional, for readability)
    averaged = averaged.sort_values(by=["MatrixSize", "TileSize"])

    # Save output (tab-separated)
    averaged.to_csv(output, index=False)
    print(f"✅ Averaged results saved to {output}")

if __name__ == "__main__":
    main()

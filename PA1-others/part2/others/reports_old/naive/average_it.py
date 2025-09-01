import pandas as pd
import sys
import re

def extract_simd_time(program_output):
    """
    Extract the number of microseconds from the 'Time WITH SIMD:' field.
    Returns None if not found.
    """
    match = re.search(r'Time WITHOUT software prefetching:\s*([\d.]+)\s*microseconds', program_output)
    if match:
        return float(match.group(1))
    return None

def average_csv(files, output_file):
    if len(files) != 3:
        print("Please provide exactly 3 CSV files.")
        return

    # Load all CSVs
    dfs = [pd.read_csv(f) for f in files]

    # Make sure all CSVs have the same columns
    if not all(dfs[0].columns.equals(df.columns) for df in dfs[1:]):
        print("CSV files do not have the same structure.")
        return

    # Initialize result with first CSV
    result = dfs[0].copy()

    # Identify numeric columns (excluding ProgramOutput)
    numeric_cols = result.select_dtypes(include='number').columns

    # Average numeric columns across all CSVs
    for col in numeric_cols:
        result[col] = (dfs[0][col] + dfs[1][col] + dfs[2][col]) / 3

    # Round and convert 'instructions' and 'cycles' columns to integers
    for col in ['instructions', 'cycles']:
        if col in result.columns:
            result[col] = result[col].round().astype(int)

    # Extract TimeElapsedMicroSeconds from ProgramOutput
    result['TimeElapsedMicroSeconds'] = result['ProgramOutput'].apply(extract_simd_time)

    # Save to output file
    result.to_csv(output_file, index=False)
    print(f"Averaged CSV with SIMD times saved to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) < 5:
        print("Usage: python average_csv.py <file1.csv> <file2.csv> <file3.csv> <output.csv>")
        sys.exit(1)

    input_files = sys.argv[1:4]
    output_file = sys.argv[4]

    average_csv(input_files, output_file)

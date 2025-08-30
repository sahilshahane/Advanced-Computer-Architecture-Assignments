import re
import pandas as pd
import os
import sys

# --- Get folder path and output file path from command line arguments ---
if len(sys.argv) < 3:
    print("Usage: python extract_data.py <input_folder> <output_csv>")
    sys.exit(1)

INPUT_FOLDER = sys.argv[1]
OUTPUT_FILE = sys.argv[2]

rows = []

# Regex to match filenames like perf_tile2_N256.log
filename_pattern = re.compile(r"perf_tile(\d+)_N(\d+)\.log")

for fname in os.listdir(INPUT_FOLDER):
    match = filename_pattern.match(fname)
    if not match:
        continue

    TileSize, MatrixSize = match.group(1), match.group(2)
    filepath = os.path.join(INPUT_FOLDER, fname)

    with open(filepath, "r") as f:
        raw_text = f.read()

    # Extract execution times (we only need SIMD time for TimeElapsedMs)
    timing_pattern = re.compile(
        r"Normal matrix multiplication took (\d+) ms.*?\n\s*\n?SIMD matrix multiplication took (\d+) ms.*?\n.*?Normalized performance:\s*([0-9.]+)",
        re.S,
    )
    tmatch = timing_pattern.search(raw_text)
    simd_ms = int(tmatch.group(2)) if tmatch else 0

    # Map metrics to column names we want
    metric_map = {
        "instructions": "Instructions",
        "cycles": "Cycles",
        "L1-dcache-loads": "L1-dcache-loads",
        "L1-dcache-load-misses": "L1-dcache-load-misses",
        "branch-instructions": "Branch-instructions",
        "branch-misses": "Branch-misses",
        "cache-references": "Cache-references",
        "cache-misses": "Cache-misses",
        "context-switches": "Context-switches",
        # "task-clock": "TimeElapsedMs",  # we ignore and override with simd_ms
    }

    metrics = {}
    for line in raw_text.strip().splitlines():
        if ",," not in line:
            continue
        parts = line.split(",,")
        if len(parts) != 2:
            continue
        left = parts[0].split(",")  # e.g., ["71220230"]
        right = parts[1].split(",")  # e.g., [metric, event_id, percent, extra_value, extra_label]

        try:
            value = float(left[0])
        except ValueError:
            continue
        metric = right[0].strip()

        if metric in metric_map:
            metrics[metric_map[metric]] = value

            # --- Derived fields from the trailing columns (indexes based on perf CSV pattern) ---
            # right[3] usually holds the numeric 'extra' (e.g., IPC or miss-rate), right[4] the label
            if metric == "instructions" and len(right) >= 4:
                # 'insn per cycle' is provided with the instructions line
                try:
                    metrics["IPC"] = float(right[3])
                except ValueError:
                    pass
            if metric == "L1-dcache-load-misses" and len(right) >= 4:
                try:
                    metrics["L1-miss-rate-percent"] = float(right[3])
                except ValueError:
                    pass
            if metric == "branch-misses" and len(right) >= 4:
                try:
                    metrics["Branch-miss-rate-percent"] = float(right[3])
                except ValueError:
                    pass
            if metric == "cache-misses" and len(right) >= 4:
                try:
                    metrics["Cache-miss-rate-percent"] = float(right[3])
                except ValueError:
                    pass

    # Add fields
    metrics["TileSize"] = TileSize
    metrics["MatrixSize"] = MatrixSize
    metrics["TimeElapsedMs"] = simd_ms
    metrics["TimeElapsedSeconds"] = simd_ms / 1000.0 if simd_ms else 0.0

    # Store row
    rows.append(metrics)

# Ensure correct column order
columns = [
    "TileSize","MatrixSize","Instructions","Cycles","IPC",
    "L1-dcache-loads","L1-dcache-load-misses","L1-miss-rate-percent",
    "Branch-instructions","Branch-misses","Branch-miss-rate-percent",
    "Cache-references","Cache-misses","Cache-miss-rate-percent",
    "Context-switches","TimeElapsedSeconds",
    "TimeElapsedMs"
]

# Build DataFrame
df = pd.DataFrame(rows)
# Ensure all expected columns exist
for c in columns:
    if c not in df.columns:
        df[c] = ""

df = df.reindex(columns=columns)

# Save to CSV
df.to_csv(OUTPUT_FILE, index=False)
print(f"Extracted {len(df)} files into {OUTPUT_FILE}")

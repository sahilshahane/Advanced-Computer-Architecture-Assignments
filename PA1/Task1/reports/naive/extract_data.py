import re
import pandas as pd
import os
import sys

# if len(sys.argv) < 3:
#     print("Usage: python extract_data.py <input_parent_folder> <output_folder>")
#     sys.exit(1)

INPUT_PARENT = "reports/naive/benchmarks"
OUTPUT_FOLDER = "reports/naive/benchmarks"

if not os.path.exists(OUTPUT_FOLDER):
    os.makedirs(OUTPUT_FOLDER)

rows = []

# Regex to match filenames like perf_tile2_N256.log
filename_pattern = re.compile(r"perf_tile(\d+)_N(\d+)\.log")

# Regex for folder name: perf_logs_naive_<TIME>_<DATE>
folder_pattern = re.compile(r"perf_logs_(\w+)_(\d+)_(\d+)")

for folder in os.listdir(INPUT_PARENT):
    fmatch = folder_pattern.match(folder)
    if not fmatch:
        continue

    algo = fmatch.group(1)       # "naive" or "simd"
    time_str = fmatch.group(2)   # e.g., 164300
    date_str = fmatch.group(3)   # e.g., 29082025

    input_folder = os.path.join(INPUT_PARENT, folder)

    rows = []
    for fname in os.listdir(input_folder):
        match = filename_pattern.match(fname)
        if not match:
            continue

        TileSize, MatrixSize = match.group(1), match.group(2)
        filepath = os.path.join(input_folder, fname)

        with open(filepath, "r") as f:
            raw_text = f.read()

        # Extract naive execution time (ms)
        timing_pattern = re.compile(
            r"Normal matrix multiplication took (\d+) ms",
            re.S,
        )
        tmatch = timing_pattern.search(raw_text)
        naive_ms = int(tmatch.group(1)) if tmatch else 0

        # Map metrics
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
            "task-clock": "Task-clock",
        }

        metrics = {}
        for line in raw_text.strip().splitlines():
            if ",," not in line:
                continue
            parts = line.split(",,")
            if len(parts) != 2:
                continue
            left = parts[0].split(",")
            right = parts[1].split(",")

            try:
                value = float(left[0])
            except ValueError:
                continue
            metric = right[0].strip()

            if metric in metric_map:
                metrics[metric_map[metric]] = value

                if metric == "instructions" and len(right) >= 4:
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

        metrics["TileSize"] = TileSize
        metrics["MatrixSize"] = MatrixSize
        metrics["TimeElapsedMs"] = naive_ms
        metrics["TimeElapsedSeconds"] = naive_ms / 1000.0 if naive_ms else 0.0

        rows.append(metrics)

    # Columns
    columns = [
        "TileSize","MatrixSize","Instructions","Cycles","IPC",
        "L1-dcache-loads","L1-dcache-load-misses","L1-miss-rate-percent",
        "Branch-instructions","Branch-misses","Branch-miss-rate-percent",
        "Cache-references","Cache-misses","Cache-miss-rate-percent",
        "Context-switches","Task-clock","TimeElapsedMs","TimeElapsedSeconds"
    ]

    df = pd.DataFrame(rows)
    for c in columns:
        if c not in df.columns:
            df[c] = ""
    df = df.reindex(columns=columns)

    # Build output filename
    output_file = os.path.join(
        OUTPUT_FOLDER,
        f"{algo}_benchmark_{time_str}_{date_str}.csv"
    )

    df.to_csv(output_file, index=False, sep="\t")
    print(f"Extracted {len(df)} files into {output_file}")

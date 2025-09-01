import re
import pandas as pd
import os
import sys

# if len(sys.argv) < 3:
#     print("Usage: python extract_data.py <parent_input_folder> <output_folder>")
#     sys.exit(1)

# PARENT_INPUT_FOLDER = sys.argv[1]
# OUTPUT_FOLDER = sys.argv[2]

PARENT_INPUT_FOLDER = "reports_sahil/Task_1C/perf_logs"
OUTPUT_FOLDER = "reports_sahil/Task_1C"



# Regex for subfolder names
folder_pattern = re.compile(r"perf_logs_simd_(\d+)_(\d+)_(\d+)")
# Regex for filenames inside folders
filename_pattern = re.compile(r"perf_tile(\d+)_N(\d+)\.log")

os.makedirs(OUTPUT_FOLDER, exist_ok=True)

def process_folder(input_folder, output_file):
    rows = []

    for fname in os.listdir(input_folder):
        match = filename_pattern.match(fname)
        if not match:
            continue

        TileSize, MatrixSize = match.group(1), match.group(2)
        filepath = os.path.join(input_folder, fname)

        with open(filepath, "r") as f:
            raw_text = f.read()

        # Extract SIMD execution time
        timing_pattern = re.compile(
            r"Normal matrix multiplication took (\d+) ms.*?\n\s*\n?SIMD matrix multiplication took (\d+) ms.*?\n.*?Normalized performance:\s*([0-9.]+)",
            re.S,
        )
        tmatch = timing_pattern.search(raw_text)
        simd_ms = int(tmatch.group(2)) if tmatch else 0

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
        metrics["TimeElapsedMs"] = simd_ms
        metrics["TimeElapsedSeconds"] = simd_ms / 1000.0 if simd_ms else 0.0

        rows.append(metrics)

    columns = [
        "TileSize","MatrixSize","Instructions","Cycles","IPC",
        "L1-dcache-loads","L1-dcache-load-misses","L1-miss-rate-percent",
        "Branch-instructions","Branch-misses","Branch-miss-rate-percent",
        "Cache-references","Cache-misses","Cache-miss-rate-percent",
        "Context-switches","TimeElapsedMs","TimeElapsedSeconds"
    ]

    df = pd.DataFrame(rows)
    for c in columns:
        if c not in df.columns:
            df[c] = ""

    df = df.reindex(columns=columns)
    df.to_csv(output_file, index=False, sep="\t")
    print(f"✅ Extracted {len(df)} rows into {output_file}")

# --- Main loop: process all subfolders ---
for subfolder in os.listdir(PARENT_INPUT_FOLDER):
    full_path = os.path.join(PARENT_INPUT_FOLDER, subfolder)
    if not os.path.isdir(full_path):
        continue

    m = folder_pattern.match(subfolder)
    if not m:
        continue

    reg_size, time_str, date_str = m.groups()
    output_filename = f"simd_{reg_size}_benchmark_{time_str}_{date_str}.csv"
    output_file = os.path.join(OUTPUT_FOLDER, output_filename)

    process_folder(full_path, output_file)

#!/usr/bin/env python3
import re
import csv
import sys
import os

def parse_file(file_path):
    matmul_results = {}
    nn_results = {}
    matrix_size = None

    with open(file_path, "r") as f:
        lines = f.readlines()

    section = None
    for line in lines:
        line = line.strip()

        # Matrix size
        m_size = re.match(r"Matrix size:\s*(\d+)x(\d+)", line)
        if m_size:
            matrix_size = m_size.group(1)

        # Determine section
        if "=== Matrix Multiplication" in line:
            section = "matmul"
            continue
        if "=== Neural Network" in line:
            section = "nn"
            continue

        # Parse Matrix Multiplication
        if section == "matmul":
            # Timing
            m_time = re.match(r"([A-Za-z/() ]+):\s*([\d.]+)\s*ms", line)
            if m_time:
                algo = m_time.group(1).strip()
                time_ms = float(m_time.group(2))
                matmul_results[algo] = {"TimeMs": time_ms, "SpeedUp": None}

            # Speedup
            m_speed = re.match(r"([A-Za-z/() ]+):\s*([\d.]+)x", line)
            if m_speed:
                algo = m_speed.group(1).strip()
                speedup = float(m_speed.group(2))
                if algo in matmul_results:
                    matmul_results[algo]["SpeedUp"] = speedup

        # Parse Neural Network
        if section == "nn":
            m_nn = re.match(r".*?([A-Za-z]+)\s*-\s*Time:\s*([\d.]+)\s*ms\s*-\s*Final Loss:\s*([\d.]+)", line)
            m_total = re.match(r"Total loss:\s*([\d.]+)", line)
            if m_total:
                total_loss = float(m_total.group(1))
            if m_nn:
                algo_nn = m_nn.group(1).strip()
                nn_time = float(m_nn.group(2))
                nn_results[algo_nn.lower()] = {"NNTime": nn_time, "TotalLoss": total_loss}

    # Mapping Matrix Multiplication names to NN names
    name_map = {
        "Basic (ijk)": "basic",
        "Reordered (ikj)": "reordered",
        "Unrolled": "unrolled",
        "Tiled/Blocked": "tiled",
        "SIMD Vectorized": "vectorized"
    }

    # Combine results
    combined = []
    for algo, data in matmul_results.items():
        nn_key = name_map.get(algo)
        nn_data = nn_results.get(nn_key, {})
        combined.append([
            matrix_size,
            algo,
            data["TimeMs"],
            data["SpeedUp"],
            nn_data.get("TotalLoss"),
            nn_data.get("NNTime")
        ])
    return combined

def process_folder(folder_path, output_file):
    all_results = []

    # Iterate all files in folder
    for filename in os.listdir(folder_path):
        if filename.endswith(".log"):  # only process txt files
            file_path = os.path.join(folder_path, filename)
            results = parse_file(file_path)
            all_results.extend(results)

    # Write CSV
    with open(output_file, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Matrix Size", "Algorithm Type", "TimeMs", "SpeedUp over Basic", "Total Loss", "NN Time"])
        writer.writerows(all_results)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python extract_benchmark_folder.py <folder_path> <output.csv>")
        sys.exit(1)

    process_folder(sys.argv[1], sys.argv[2])

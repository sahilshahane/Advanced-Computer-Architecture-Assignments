#!/bin/bash

# --- Configuration ---
EXECUTABLE="./a.out"
CSV_FILE="loop_unroll_loop_optimize_results.csv"
PERF_EVENTS="l2_rqsts.miss,L1-dcache-load-misses,instructions,cycles,sw_prefetch_access.any,L1-dcache-store-misses,cache-misses,LLC-load-misses"
PARAMETERS=(256 300 400 512 800 1024 1200 1500 1800 2000 2048 2400 2700 3000 257 301 401 513 801 1025 1201 1501 1801 2001 2049 2401 2701 3001)

# --- Script Logic ---

if [ ! -x "$EXECUTABLE" ]; then
    echo "Error: Executable '$EXECUTABLE' not found or is not executable."
    exit 1
fi

echo "Starting Performance Test Automation Script..."
echo "Results will be logged in: $CSV_FILE"
echo "----------------------------------------------------"

HEADER="Parameter,${PERF_EVENTS//,/,},ExecutableOutput"
echo "$HEADER" > "$CSV_FILE"

for param in "${PARAMETERS[@]}"; do
    
    echo "Running test for Parameter = ${param}..."
    
    # Run the command and capture both stdout (from C++ app) and stderr (from perf)
    FULL_OUTPUT=$(perf stat -e "$PERF_EVENTS" "$EXECUTABLE" "$param" 2>&1)

    # Extract the C++ program's output by taking everything BEFORE the perf summary
    EXECUTABLE_OUTPUT=$(echo "$FULL_OUTPUT" | sed '/Performance counter stats/q' | sed '$d')
    
    # Extract the perf data by taking everything AFTER the perf summary line
    PERF_DATA=$(echo "$FULL_OUTPUT" | sed -n '/Performance counter stats/,$p')
    
    CSV_ROW="${param}"

    IFS=',' read -ra EVENT_ARRAY <<< "$PERF_EVENTS"
    for event in "${EVENT_ARRAY[@]}"; do
        # ROBUST PARSING:
        # 1. Use grep to find the line containing the event name.
        # 2. Use awk to print the very first column (the number).
        # 3. Use tr to delete any commas from the number.
        value=$(echo "$PERF_DATA" | grep -w "$event" | awk '{print $1}' | tr -d ',')
        
        # If no value was found, use 0.
        CSV_ROW="${CSV_ROW},${value:-0}"
    done

    # Sanitize by removing newlines and quoting.
    SANITIZED_OUTPUT=$(echo "$EXECUTABLE_OUTPUT" | tr '\n' ' ')
    CSV_ROW="${CSV_ROW},\"${SANITIZED_OUTPUT}\""

    echo "$CSV_ROW" >> "$CSV_FILE"

done

echo "----------------------------------------------------"
echo "All tests completed."
echo "Full results have been saved to '$CSV_FILE'."
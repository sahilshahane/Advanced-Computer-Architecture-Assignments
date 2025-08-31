#!/bin/bash

# --- Configuration ---
EXECUTABLE="./a.out"
CSV_FILE="loop_unroll_loop_optimize_results.csv"
PERF_EVENTS="l2_rqsts.miss,L1-dcache-load-misses,instructions,cycles,sw_prefetch_access.any,cache-misses,LLC-load-misses,context-switches,branch-instructions,branch-misses,cache-references,task-clock"

PARAMETERS=(256 300 400 512 800 1024 1200 1500 1800 2000 2048 2400 2700 3000 257 301 401 513 801 1025 1201 1501 1801 2001 2049 2401 2701 3001)

# --- Script Logic ---

if [ ! -x "$EXECUTABLE" ]; then
    echo "Error: Executable '$EXECUTABLE' not found or is not executable."
    exit 1
fi

echo "Starting Performance Test Automation Script..."
echo "Results will be logged in: $CSV_FILE"
echo "----------------------------------------------------"

# Create the CSV header
HEADER="Parameter,${PERF_EVENTS//,/,},ExecutableOutput"
echo "$HEADER" > "$CSV_FILE"

for param in "${PARAMETERS[@]}"; do
    
    echo "Running test for Parameter = ${param}..."
    
    # Use a temporary file to capture the stderr output from perf
    PERF_TEMP_FILE=$(mktemp)

    # Run the command:
    # - stdout from EXECUTABLE is captured by the variable EXECUTABLE_OUTPUT.
    # - stderr from perf is redirected to the temporary file.
    EXECUTABLE_OUTPUT=$(perf stat -x, -e "$PERF_EVENTS" "$EXECUTABLE" "$param" 2> "$PERF_TEMP_FILE")

    # --- ROBUST PARSING LOGIC ---
    # Instead of a simple cut/tr pipeline, we read the perf output line by line.
    # This correctly handles any extra informational lines or blank lines that
    # perf might output to stderr.

    values=() # Create an empty array to hold the counter values

    # Read the temporary file line by line
    while IFS=, read -r val _; do
        # This is the key: only process the line if the first field looks
        # like a number. This filters out headers, blank lines, and text like "<not counted>".
        if [[ "$val" =~ ^[0-9] ]]; then
            values+=("$val")
        fi
    done < "$PERF_TEMP_FILE"
    
    # Clean up the temporary file
    rm "$PERF_TEMP_FILE"

    # Join the array of values with a comma. This is a clean way to build
    # the comma-separated string without a trailing comma.
    PERF_VALUES=$(IFS=,; echo "${values[*]}")

    # Sanitize the executable's output by removing newlines and then quote it for CSV safety.
    SANITIZED_OUTPUT=$(echo "$EXECUTABLE_OUTPUT" | tr '\n' ' ')

    # Combine everything into the final CSV row
    CSV_ROW="${param},${PERF_VALUES},\"${SANITIZED_OUTPUT}\""

    # Append the new row to the CSV file
    echo "$CSV_ROW" >> "$CSV_FILE"

done

echo "----------------------------------------------------"
echo "All tests completed."
echo "Full results have been saved to '$CSV_FILE'."
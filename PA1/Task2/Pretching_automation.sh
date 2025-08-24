#!/bin/bash

# --- Configuration ---pr
# The executable to test.
EXECUTABLE="./a.out"
# The log file where all results will be saved.
LOG_FILE="perf_results.log"
# The list of perf events to monitor.
PERF_EVENTS="l2_rqsts.miss,L1-dcache-load-misses,instructions,cycles,sw_prefetch_access.any,L1-dcache-store-misses,cache-misses,LLC-load-misses"

# --- Parameters to Test ---
# An array of prefetch distances.
PREFETCH_DISTANCES=(4 8 12 16 20 24 28)
# An array of embedding table sizes.
TABLE_SIZES=(1000000 6000000 10000000)
# An array of cache fill levels (locality hints for __builtin_prefetch).
LOCALITY_HINTS=(0 1 2 3)

# --- Script Logic ---

# Check if the executable exists and is actually executable.
if [ ! -x "$EXECUTABLE" ]; then
    echo "Error: Executable '$EXECUTABLE' not found or is not executable."
    echo "Please compile your C++ code first (e.g., g++ your_code.cpp -o a.out)"
    exit 1
fi

# Clear the log file from previous runs and write a header.
echo "Starting Performance Test Automation Script" > "$LOG_FILE"
echo "Results will be logged in: $LOG_FILE"
echo "----------------------------------------------------"
echo ""

# Loop through each combination of parameters.
for size in "${TABLE_SIZES[@]}"; do
    for distance in "${PREFETCH_DISTANCES[@]}"; do
        for locality in "${LOCALITY_HINTS[@]}"; do
            
            # --- Prepare the command and log headers ---
            HEADER="===== RUNNING TEST: Size=${size}, Distance=${distance}, Locality=${locality} ====="
            
            # Print the header to the console for real-time progress.
            echo "$HEADER"
            
            # Append the header to the log file.
            echo "$HEADER" >> "$LOG_FILE"
            
            # Construct the full perf command.
            # Note: Your ./a.out program MUST be able to accept these three arguments.
            COMMAND="perf stat -e $PERF_EVENTS $EXECUTABLE $distance $size $locality"
            
            # --- Execute the command ---
            # We use '2>&1' to redirect stderr (where perf prints results) to stdout,
            # and then pipe it to 'tee -a' which appends to the log file AND prints to the console.
            # Using 'script' command can also be a robust way to capture output.
            script -q -c "$COMMAND" /dev/null | tee -a "$LOG_FILE"

            # Add a separator to the log file for better readability.
            echo "" >> "$LOG_FILE"
            echo "" 
        done
    done
done

echo "----------------------------------------------------"
echo "All tests completed."
echo "Full results have been saved to '$LOG_FILE'."

#!/bin/bash

# --- Configuration ---
# The executable to test.
EXECUTABLE="./a.out"
# The log file where all results will be saved.
LOG_FILE="loop_unroll_loop_optimize.log"
# The list of perf events to monitor.
PERF_EVENTS="l2_rqsts.miss,L1-dcache-load-misses,instructions,cycles,sw_prefetch_access.any,L1-dcache-store-misses,cache-misses,LLC-load-misses"

# --- Parameters to Test ---
# An array of single parameters to pass to the executable.
# These will likely be your prefetch distances.
PARAMETERS=(8 16 32 64 96 128 192 256 384 512 768 1024 1536 2048)

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

# Loop through each parameter.
for param in "${PARAMETERS[@]}"; do
    
    # --- Prepare the command and log headers ---
    HEADER="===== RUNNING TEST: Parameter = ${param} ====="
    
    # Print the header to the console for real-time progress.
    echo "$HEADER"
    
    # Append the header to the log file.
    echo "$HEADER" >> "$LOG_FILE"
    
    # Construct the full perf command.
    # Your ./a.out program must be able to accept this single argument.
    COMMAND="perf stat -e $PERF_EVENTS $EXECUTABLE $param"
    
    # --- Execute the command ---
    # Redirect stderr (where perf prints) to stdout, then pipe to 'tee'
    # to append to the log file AND print to the console.
    $COMMAND 2>&1 | tee -a "$LOG_FILE"

    # Add a separator for better readability.
    echo "" >> "$LOG_FILE"
    echo "" 
done

echo "----------------------------------------------------"
echo "All tests completed."
echo "Full results have been saved to '$LOG_FILE'."

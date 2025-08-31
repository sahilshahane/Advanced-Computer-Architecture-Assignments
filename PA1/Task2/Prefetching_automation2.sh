#!/bin/bash

# --- Configuration ---
CPP_SOURCE_FILE="emb.cpp"
EXECUTABLE="./a.out"
CSV_FILE="perf_results_final.csv"
PERF_EVENTS="l2_rqsts.miss,L1-dcache-load-misses,instructions,cycles,sw_prefetch_access.any,cache-misses,mem_load_retired.l3_miss,LLC-load-misses,context-switches,branch-instructions,branch-misses,cache-references,task-clock"

# --- Parameters to Test ---
PREFETCH_DISTANCES=(4 8 12 16 20 24 28)
TABLE_SIZES=(1000000 6000000 10000000)
LOCALITY_HINTS=(0 1 2 3) # The different cache fill levels to test

# --- Script Logic ---

# Create the CSV header with all three parameter columns.
echo "LocalityHint,TableSize,PrefetchDistance,${PERF_EVENTS}" > "$CSV_FILE"

echo "Starting automated compile-and-test process..."
echo "Results will be logged to $CSV_FILE"
echo "----------------------------------------------------"

# Outer loop: Iterate through each locality hint.
for locality in "${LOCALITY_HINTS[@]}"; do
    
    echo ""
    echo "--- Compiling for Locality Hint: $locality ---"

    # --- COMPILE STEP ---
    # Recompile the program, passing the locality hint as a compile-time definition (-D).
    g++ "$CPP_SOURCE_FILE" -Wno-write-strings -msse2 -mavx  -mavx512f -O2 -g -DLOCALITY_HINT=$locality -o "$EXECUTABLE"
    
    # Check if compilation was successful.
    if [ $? -ne 0 ]; then
        echo "Error: Compilation failed for locality hint $locality. Aborting."
        exit 1
    fi

    # Middle loop: Iterate through each table size.
    for size in "${TABLE_SIZES[@]}"; do
        
        # Inner loop: Iterate through each prefetch distance.
        for distance in "${PREFETCH_DISTANCES[@]}"; do
            
            echo "Running test: Locality=$locality, Size=$size, Distance=$distance"

            # --- EXECUTION STEP (MODIFIED) ---
            # Now pipes only the two required values: distance and size.
            PERF_OUTPUT=$(echo "$distance $size" | perf stat -x, -e "$PERF_EVENTS" "$EXECUTABLE" 2>&1)

            # Parse the output to get just the numeric values.
            VALUES=$(echo "$PERF_OUTPUT" | grep -v '^#' | grep . | awk -F, '{print $1}' | paste -sd ',')

            if [ -n "$VALUES" ]; then
                # Append the parameters and perf values to the CSV.
                echo "$locality,$size,$distance,$VALUES" >> "$CSV_FILE"
            else
                echo "Warning: Failed to collect perf data for Locality=$locality, Size=$size, Distance=$distance."
                echo "$locality,$size,$distance," >> "$CSV_FILE"
            fi
        done
    done
done

echo "----------------------------------------------------"
echo "All tests completed."
echo "Results have been saved to '$CSV_FILE'."

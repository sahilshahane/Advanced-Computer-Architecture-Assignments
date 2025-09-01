#!/bin/bash
# set -euo pipefail


timestamp=$(date +"%H%M%S_%d%m%Y")

# --- Configuration ---
CPP_SOURCE_FILE="emb.cpp"
EXECUTABLE="./emb_naive"
NAIVE_CSV_FILE="perf_results_naive_${timestamp}.csv"
PREFETCH_CSV_FILE="perf_results_prefetch_${timestamp}.csv"
PERF_EVENTS="l2_rqsts.miss,L1-dcache-load-misses,instructions,cycles,sw_prefetch_access.any,cache-misses,mem_load_retired.l3_miss,LLC-load-misses,context-switches,branch-instructions,branch-misses,cache-references,task-clock"

# --- Parameters to Test ---
CODE_VARIANTS=("NAIVE")
PREFETCH_DISTANCES=(4 8 12 16 20 24 28)
INPUT_SIZES=(128 192 320 512 640 784 896 1024)
TABLE_SIZES=(50000 150000 250000 500000 1000000)
LOCALITY_HINTS=(0)

# --- Script Logic ---
if ! command -v perf &> /dev/null; then
    echo "Error: 'perf' command not found." && exit 1
fi

# --- MODIFICATION: Add the new 'ProgramOutput' column to the headers ---
echo "LocalityHint,TableSize,InputSize,${PERF_EVENTS},ProgramOutput" > "$NAIVE_CSV_FILE"

echo "Starting automated compile-and-test process..."
echo "Results will be logged to $NAIVE_CSV_FILE and $PREFETCH_CSV_FILE"
echo "===================================================="

for variant in "${CODE_VARIANTS[@]}"; do
    if [ "$variant" == "NAIVE" ]; then
        OUTPUT_CSV="$NAIVE_CSV_FILE"
    else
        OUTPUT_CSV="$PREFETCH_CSV_FILE"
    fi

    for locality in "${LOCALITY_HINTS[@]}"; do
        for table_size in "${TABLE_SIZES[@]}"; do
            for input_size in "${INPUT_SIZES[@]}"; do
                
                # --- MODIFICATION: Create a temporary file to capture program output ---
                EXEC_OUTPUT_FILE=$(mktemp)

                if [ "$variant" == "PREFETCH" ]; then
                    for distance in "${PREFETCH_DISTANCES[@]}"; do
                        printf "Running test: V=%s L=%s TblSize=%-7d InpSize=%-4d Dist=%-2d -> %s\n" \
                            "$variant" "$locality" "$table_size" "$input_size" "$distance" "$OUTPUT_CSV"
                        
                        # --- MODIFICATION: Run executable in a subshell to capture its output ---
                        PERF_OUTPUT=$(echo "$distance $table_size $input_size" | perf stat -x, -e "$PERF_EVENTS" \
                            sh -c "$EXECUTABLE > $EXEC_OUTPUT_FILE 2>&1" \
                            2>&1)
                        
                        VALUES=$(echo "$PERF_OUTPUT" | grep ',' | grep -v '^#' | awk -F, '{print $1}' | paste -sd ',')
                        
                        # --- MODIFICATION: Read and sanitize the program's output ---
                        EXEC_OUTPUT=$(cat "$EXEC_OUTPUT_FILE" | tr '\n' ' ' | tr -d ',')
                        
                        # --- MODIFICATION: Add the sanitized output as the last column ---
                        echo "$locality,$table_size,$input_size,$distance,$VALUES,\"$EXEC_OUTPUT\"" >> "$OUTPUT_CSV"
                    done
                else # NAIVE case
                    printf "Running test: V=%s L=%s TblSize=%-7d InpSize=%-4d -> %s\n" \
                        "$variant" "$locality" "$table_size" "$input_size" "$OUTPUT_CSV"

                    # --- MODIFICATION: Run executable in a subshell to capture its output ---
                    PERF_OUTPUT=$(echo "0 $table_size $input_size" | perf stat -x, -e "$PERF_EVENTS" \
                        sh -c "$EXECUTABLE > $EXEC_OUTPUT_FILE 2>&1" \
                        2>&1)
                        
                    VALUES=$(echo "$PERF_OUTPUT" | grep ',' | grep -v '^#' | awk -F, '{print $1}' | paste -sd ',')
                    
                    # --- MODIFICATION: Read and sanitize the program's output ---
                    EXEC_OUTPUT=$(cat "$EXEC_OUTPUT_FILE" | tr '\n' ' ' | tr -d ',')
                    
                    # --- MODIFICATION: Add the sanitized output as the last column ---
                    echo "$locality,$table_size,$input_size,$VALUES,\"$EXEC_OUTPUT\"" >> "$OUTPUT_CSV"
                fi
                
                # --- MODIFICATION: Clean up the temporary file ---
                rm -f "$EXEC_OUTPUT_FILE"

            done
        done
    done
done

echo "===================================================="
echo "All tests completed."
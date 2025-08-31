#!/bin/bash
# set -euo pipefail

timestamp=$(date +"%H%M%S_%d%m%Y")

# --- Configuration ---
CPP_SOURCE_FILE="emb.cpp"
NAIVE_CSV_FILE="perf_results_naive_${timestamp}.csv"
PREFETCH_CSV_FILE="perf_results_prefetch_${timestamp}.csv"
SIMD_128_CSV_FILE="perf_results_simd_128_${timestamp}.csv"
SIMD_256_CSV_FILE="perf_results_simd_256_${timestamp}.csv"
SIMD_512_CSV_FILE="perf_results_simd_512_${timestamp}.csv"
PERF_EVENTS="l2_rqsts.miss,L1-dcache-load-misses,instructions,cycles,sw_prefetch_access.any,cache-misses,mem_load_retired.l3_miss,LLC-load-misses,context-switches,branch-instructions,branch-misses,cache-references,task-clock"

# --- Parameters to Test ---
# CODE_VARIANTS=("PREFETCH")
# CODE_VARIANTS=("NAIVE")
CODE_VARIANTS=("SIMD_512")
PREFETCH_DISTANCES=(4 8 12 16 20 24 28)
INPUT_SIZES=(90 180 360 720 1024)
TABLE_SIZES=(125000 250000 500000 1000000 6000000)
LOCALITY_HINTS=(0)

# --- Script Logic ---
if ! command -v perf &> /dev/null; then
    echo "Error: 'perf' command not found." && exit 1
fi

echo "Starting automated compile-and-test process..."
echo "Results will be logged to $NAIVE_CSV_FILE and $PREFETCH_CSV_FILE"
echo "===================================================="

for variant in "${CODE_VARIANTS[@]}"; do
    if [ "$variant" == "NAIVE" ]; then
        OUTPUT_CSV="$NAIVE_CSV_FILE"
        echo "LocalityHint,TableSize,InputSize,${PERF_EVENTS},ProgramOutput" > "$NAIVE_CSV_FILE"
    elif [ "$variant" == "PREFETCH" ]; then
        OUTPUT_CSV="$PREFETCH_CSV_FILE"
        echo "LocalityHint,TableSize,InputSize,PrefetchDistance,${PERF_EVENTS},ProgramOutput" > "$PREFETCH_CSV_FILE"
    elif [ "$variant" == "SIMD_128" ]; then
        OUTPUT_CSV="$SIMD_128_CSV_FILE"
        EXECUTABLE="./emb_128"
        echo "LocalityHint,TableSize,InputSize,${PERF_EVENTS},ProgramOutput" > "$OUTPUT_CSV"
    elif [ "$variant" == "SIMD_256" ]; then
        OUTPUT_CSV="$SIMD_256_CSV_FILE"
        EXECUTABLE="./emb_256"
        echo "LocalityHint,TableSize,InputSize,${PERF_EVENTS},ProgramOutput" > "$OUTPUT_CSV"
    elif [ "$variant" == "SIMD_512" ]; then
        OUTPUT_CSV="$SIMD_512_CSV_FILE"
        EXECUTABLE="./emb_512"
        echo "LocalityHint,TableSize,InputSize,${PERF_EVENTS},ProgramOutput" > "$OUTPUT_CSV"
    else
        echo "Unknown code variant: $variant" && exit 1
    fi # This 'fi' closes the main if-elif-else block for the variant

    for locality in "${LOCALITY_HINTS[@]}"; do
        for table_size in "${TABLE_SIZES[@]}"; do
            for input_size in "${INPUT_SIZES[@]}"; do

                EXEC_OUTPUT_FILE=$(mktemp)

               if [[ "$variant" == "SIMD_128" || "$variant" == "SIMD_256" || "$variant" == "SIMD_512" ]]; then
                    printf "Running test: V=%s L=%s TblSize=%-7d InpSize=%-4d -> %s\n" \
                        "$variant" "$locality" "$table_size" "$input_size" "$OUTPUT_CSV"

                    PERF_OUTPUT=$(echo "0 $table_size $input_size" | perf stat -x, -e "$PERF_EVENTS" \
                        sh -c "$EXECUTABLE > $EXEC_OUTPUT_FILE 2>&1" \
                        2>&1)

                    VALUES=$(echo "$PERF_OUTPUT" | grep ',' | grep -v '^#' | awk -F, '{print $1}' | paste -sd ',')

                    EXEC_OUTPUT=$(cat "$EXEC_OUTPUT_FILE" | tr '\n' ' ' | tr -d ',')

                    echo "$locality,$table_size,$input_size,$VALUES,\"$EXEC_OUTPUT\"" >> "$OUTPUT_CSV"
                fi

                rm -f "$EXEC_OUTPUT_FILE"

            done
        done
    done
done

echo "===================================================="
echo "All tests completed."

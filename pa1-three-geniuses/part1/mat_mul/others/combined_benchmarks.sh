#!/bin/bash

sizes=(400 512 800 801 1200 1500 1800 2000 2048 2400)
output="combined_1d_results.csv"

# Write CSV header
echo "SIZE,simd+tiling,simd+loop_optimisation,tiling+loop_optimisation,simd+loop_optimisation+tiling" > $output

for SIZE in "${sizes[@]}"; do
    echo "Running for SIZE=$SIZE..."

    # --- 1. simd+tiling ---
    best_st=""
    for T in {4,8,12,16,20,24,28,32,36,40,44,48,52,56,60}; do
        val=$(./mat_mul_combined/simd+tiling/T${T} $SIZE | grep "Combined optimization matrix multiplication took" | awk '{print $6}' | tr -d 'ms')
        if [[ -n "$val" && ( -z "$best_st" || "$val" -lt "$best_st" ) ]]; then
            best_st=$val
        fi
    done

    # --- 2. simd+loop_optimisation ---
    best_sl=""
    for U in {8,16,20,28,32,36,40,44,48,64,80,128,156,192}; do
        val=$(./mat_mul_combined/simd+loop_optimisation/U${U} $SIZE | grep "Combined optimization matrix multiplication took" | awk '{print $6}' | tr -d 'ms')
        if [[ -n "$val" && ( -z "$best_sl" || "$val" -lt "$best_sl" ) ]]; then
            best_sl=$val
        fi
    done

    # --- 3. tiling+loop_optimisation ---
    best_tl=""
    for T in {8,12,20,24,32}; do
        for U in {8,16,20,28,32,36,40,44,48,64,80,128,156,192}; do
            val=$(./mat_mul_combined/tiling+loop_optmisation/T${T}_U${U} $SIZE | grep "Combined optimization matrix multiplication took" | awk '{print $6}' | tr -d 'ms')
            if [[ -n "$val" && ( -z "$best_tl" || "$val" -lt "$best_tl" ) ]]; then
                best_tl=$val
            fi
        done
    done

    # --- 4. simd+loop_optimisation+tiling ---
    best_slt=""
    for T in {8,12,20,24,32}; do
        for U in {8,16,20,28,32,36,40,44,48,64,80,128,156,192}; do
            val=$(./mat_mul_combined/simd+loop_optimisation+tiling/T${T}_U${U} $SIZE | grep "Combined optimization matrix multiplication took" | awk '{print $6}' | tr -d 'ms')
            if [[ -n "$val" && ( -z "$best_slt" || "$val" -lt "$best_slt" ) ]]; then
                best_slt=$val
            fi
        done
    done

    # Append best results for this size to CSV
    echo "$SIZE,$best_st,$best_sl,$best_tl,$best_slt" >> $output
done

echo "Benchmarking done! Results saved in $output"

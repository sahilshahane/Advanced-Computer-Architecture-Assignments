#!/bin/bash

# output CSV file
outfile="benchmark_results.csv"

# write header
echo "Size,simd+loop_optimisation+tiling,tiling+loop_optimisation,simd+loop_optimisation,simd+tiling" > $outfile

# choose matrix sizes to test
for SIZE in 256 300 400 512 759 801 1500 1501 1800 2048 2400 3000; do
    echo "Running benchmarks for size=$SIZE"

    # run each binary with /usr/bin/time (measure real time in seconds)
    t1=$( { /usr/bin/time -f "%e" ./mat_mul_combined/simd+loop_optimisation+tiling/T8_U8 $SIZE 2>&1 1>/dev/null; } )
    t2=$( { /usr/bin/time -f "%e" ./mat_mul_combined/tiling+loop_optmisation/T8_U8 $SIZE 2>&1 1>/dev/null; } )
    t3=$( { /usr/bin/time -f "%e" ./mat_mul_combined/simd+loop_optimisation/8 $SIZE 2>&1 1>/dev/null; } )
    t4=$( { /usr/bin/time -f "%e" ./mat_mul_combined/simd+tiling/8 $SIZE 2>&1 1>/dev/null; } )

    # append row to CSV
    echo "$SIZE,$t1,$t2,$t3,$t4" >> $outfile
done

echo "✅ Benchmark finished. Results saved in $outfile"

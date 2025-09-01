#!/bin/bash

mkdir -p mat_mul_combined  # ensure base output dir exists
mkdir -p mat_mul_combined/simd+tiling  # ensure output dir exists
mkdir -p mat_mul_combined/tiling+loop_optmisation  # ensure output dir exists
mkdir -p mat_mul_combined/simd+loop_optimisation  # ensure output dir exists
mkdir -p mat_mul_combined/simd+loop_optimisation+tiling  # ensure output dir exists

for T in {4,8,12,16,20,24,28,32,36,40,44,48,52,56,60}; do 
    echo "Compiling with TILE_SIZE=$T"
    g++ -O2 -mavx512f -mfma mat_mul.c -D simd_tiling -D OPTIMIZE_COMBINED -D TILE_SIZE=$T -o mat_mul_combined/simd+tiling/T${T}
done
for U in {8,16,20,28,32,36,40,44,48,64,80,128,156,192}; do
    echo "Compiling with TILE_SIZE=$U"
    g++ -O2 -mavx512f -mfma mat_mul.c -D simd_loop_optimisation -D OPTIMIZE_COMBINED -D UNROLL_FACTOR=$U -o mat_mul_combined/simd+loop_optimisation/U${U}
done
for T in {8,12,20,24,32}; do
  for U in {8,16,20,28,32,36,40,44,48,64,80,128,156,192}; do
    echo "Compiling with TILE_SIZE=$T UNROLL_FACTOR=$U"
    g++ -O2 mat_mul.c \
        -D tiling_loop_optmisation -D OPTIMIZE_COMBINED  -D TILE_SIZE=$T -D UNROLL_FACTOR=$U \
        -o mat_mul_combined/tiling+loop_optmisation/T${T}_U${U}
  done
done
for T in {8,12,20,24,32}; do
  for U in {8,16,20,28,32,36,40,44,48,64,80,128,156,192}; do
    echo "Compiling with TILE_SIZE=$T UNROLL_FACTOR=$U"
    g++ -O2 -mavx512f -mfma mat_mul.c \
        -D simd_loop_optimisation_tiling -D OPTIMIZE_COMBINED -D TILE_SIZE=$T -D UNROLL_FACTOR=$U \
        -o mat_mul_combined/simd+loop_optimisation+tiling/T${T}_U${U}
  done
done


echo "✅ All binaries generated in mat_mul_Tiling/"

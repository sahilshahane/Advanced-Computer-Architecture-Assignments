#!/bin/bash

mkdir -p mat_mul_Tiling   # ensure output dir exists

for T in {2..200}; do
    echo "Compiling with TILE_SIZE=$T"
    g++ -O2 -g mat_mul.cpp -D OPTIMIZE_TILING -DTILE_SIZE=$T -o mat_mul_Tiling/tiling_$T
done

echo "✅ All binaries generated in mat_mul_Tiling/"
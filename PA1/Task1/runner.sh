#!/usr/bin/env bash
set -e  # Exit on any command failure

# if [ "$#" -lt 1 ]; then
#   echo "Usage: $0 <integer1> [integer2 ...]"
#   exit 1
# fi
# sizes=("$@")


sizes=(8 16 32 64 96 128 192 256 384 512 768 1024 1536 2048) # 3072 4096
reports_dir="reports"
task_variant="tiling"
register_size=""

# Create reports directory if it doesn't exist
mkdir -p "$reports_dir"

echo "Building SIMD version..."
make $task_variant

for n in "${sizes[@]}"; do
  safe_n="${n//[^0-9]/}"  # Digits only for filename
  outfile="$reports_dir/${task_variant}_${safe_n}.out"
  # outfile="$reports_dir/${task_variant}_${register_size}_${safe_n}.out"
  
  echo "Running perf for size $n..."
  perf stat --no-big-num --field-separator , --log-fd 1 -e instructions ./mat_mul/${task_variant} "$n" &> "$outfile"
  
  echo " → Results saved in $outfile"
done

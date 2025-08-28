#!/usr/bin/env bash
# set -euo pipefail

# Get current date & time in compact YYYYMMDD_HHMMSS format
timestamp=$(date +"%H%M%S_%d%m%Y")


# ---------- CONFIG ----------
OUTFILE="combination_benchmark_${timestamp}.csv"
LOGDIR="perf_logs_combination_${timestamp}"
TILES=({2..200})                # change as you like
read -r -a SIZES < "array_sizes.txt"            # change as you like
EVENTS=$(cat perf_events.txt | tr '\n' ',')  # comma-separated list of events
BIN_PATH="./mat_mul/combination"     # folder containing tiling_<tile> executables
# ----------------------------


echo "Array Sizes: ${SIZES[@]}"
echo "Perf Events: ${EVENTS}"

mkdir -p "$LOGDIR"

# Stable, non-localized output (no digit grouping, dot decimal point)
export LC_ALL=C
export LANG=C

# CSV header (numeric only, no % symbols embedded)
if [ ! -f "$OUTFILE" ]; then
  printf "TileSize\tMatrixSize\tInstructions\tCycles\tIPC\tL1-dcache-loads\tL1-dcache-load-misses\tL1-miss-rate-percent\tBranch-instructions\tBranch-misses\tBranch-miss-rate-percent\tCache-references\tCache-misses\tCache-miss-rate-percent\tContext-switches\tTimeElapsedSeconds\n" > "$OUTFILE"
fi

# Helper: extract numeric count for a given event name from perf CSV (-x,)
# Accepts suffixes like ":u" / ":k". Treats "not counted/supported" as zero.
extract_count() {
  local event="$1" input="$2"
  # perf -x, columns are generally: value , unit , event , runtime , metric
  # we match event in column 3, optionally with perf suffix (e.g., ":u")
  awk -F',' -v ev="$event" '
    function clean_num(v) {
      gsub(/^[[:space:]]+|[[:space:]]+$/, "", v)
      gsub(/,/,"", v)           # remove thousand separators if any slipped through
      return v
    }
    # Match exact event with optional perf suffix (e.g., instructions or instructions:u)
    $3 ~ ("^"ev "(:[^,]+)?$") {
      # $1 can be a number or "<not counted>" / "<not supported>"
      if ($1 ~ /^</) { print 0; exit }
      print clean_num($1); exit
    }
  ' <<< "$input"
}

# Compute safe ratios using awk (no divide-by-zero)
ratio() {
  awk -v n="$1" -v d="$2" 'BEGIN { if (d+0==0) { printf "0" } else { printf "%.6f", n/d } }'
}
percent() {
  awk -v n="$1" -v d="$2" 'BEGIN { if (d+0==0) { printf "0.00" } else { printf "%.2f", (100.0*n)/d } }'
}

for tile in "${TILES[@]}"; do
  BIN="$BIN_PATH"
  if [ ! -x "$BIN" ]; then
    echo "ERROR: Binary not found or not executable: $BIN" >&2
    exit 1
  fi

  for N in "${SIZES[@]}"; do
    echo "========================================"
    echo "Running tile=$tile, N=$N ..."
    echo "========================================"

    # Run perf with consistent output; capture stderr (perf writes to stderr)
    # --no-big-num disables digit grouping; --all-user matches your ':u' use.
    PERF_OUTPUT="$(perf stat --all-user --no-big-num -x, -e "$EVENTS" -- "$BIN" "$N" 2>&1 || true)"

    # Save raw output for debugging this run
    LOGFILE="$LOGDIR/perf_tile${tile}_N${N}.log"
    printf "%s\n" "$PERF_OUTPUT" > "$LOGFILE"

    # Extract raw counts
    instructions="$(extract_count instructions         "$PERF_OUTPUT")"; instructions="${instructions:-0}"
    cycles="$(extract_count cycles                     "$PERF_OUTPUT")"; cycles="${cycles:-0}"
    ctx="$(extract_count context-switches              "$PERF_OUTPUT")"; ctx="${ctx:-0}"
    l1_loads="$(extract_count L1-dcache-loads          "$PERF_OUTPUT")"; l1_loads="${l1_loads:-0}"
    l1_misses="$(extract_count L1-dcache-load-misses   "$PERF_OUTPUT")"; l1_misses="${l1_misses:-0}"
    branches="$(extract_count branch-instructions      "$PERF_OUTPUT")"; branches="${branches:-0}"
    br_misses="$(extract_count branch-misses           "$PERF_OUTPUT")"; br_misses="${br_misses:-0}"
    cache_refs="$(extract_count cache-references       "$PERF_OUTPUT")"; cache_refs="${cache_refs:-0}"
    cache_misses="$(extract_count cache-misses         "$PERF_OUTPUT")"; cache_misses="${cache_misses:-0}"
    task_ms="$(extract_count task-clock                "$PERF_OUTPUT")"; task_ms="${task_ms:-0}"

    # Derived metrics
    ipc="$(ratio "$instructions" "$cycles")"
    l1_miss_rate="$(percent "$l1_misses" "$l1_loads")"
    br_miss_rate="$(percent "$br_misses" "$branches")"
    cache_miss_rate="$(percent "$cache_misses" "$cache_refs")"

    # Convert milliseconds to seconds (perf task-clock is msec)
    time_elapsed_sec="$(awk -v ms="$task_ms" 'BEGIN { if (ms+0==0) printf "0"; else printf "%.9f", ms/1000 }')"

    # Append to CSV
    printf "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" \
      "$tile" "$N" \
      "$instructions" "$cycles" "$ipc" \
      "$l1_loads" "$l1_misses" "$l1_miss_rate" \
      "$branches" "$br_misses" "$br_miss_rate" \
      "$cache_refs" "$cache_misses" "$cache_miss_rate" \
      "$ctx" "$time_elapsed_sec" >> "$OUTFILE"

    echo "Saved: $OUTFILE    (log: $LOGFILE)"
  done
done

echo "========================================"
echo "Benchmarking complete."
echo "Final data saved to $OUTFILE"
echo "Raw perf outputs saved in $LOGDIR/"

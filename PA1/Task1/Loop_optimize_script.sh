#!/bin/bash

# --- Configuration ---
# The executable to test.
EXECUTABLE="./a.out"
# The CSV file where all results will be saved.
CSV_FILE="loop_unroll_loop_optimize_results.csv"
# The list of perf events to monitor (comma-separated, no spaces).
PERF_EVENTS="l2_rqsts.miss,L1-dcache-load-misses,instructions,cycles,sw_prefetch_access.any,L1-dcache-store-misses,cache-misses,LLC-load-misses"

# --- Parameters to Test ---
# An array of single parameters to pass to the executable.
PARAMETERS=(8 16 32 64 96 128 192 256 384 512 768 1024 1536 2048)

# --- Script Logic ---

# Check if the executable exists and is actually executable.
if [ ! -x "$EXECUTABLE" ]; then
    echo "Error: Executable '$EXECUTABLE' not found or is not executable."
    echo "Please compile your C++ code first (e.g., g++ your_code.cpp -o a.out)"
    exit 1
fi

# --- Prepare the CSV file ---
echo "Starting Performance Test Automation Script"
echo "Results will be logged in: $CSV_FILE"
echo "----------------------------------------------------"

# Create the CSV header row, adding the new column for the executable's output.
HEADER="Parameter,${PERF_EVENTS//,/,},ExecutableOutput"
echo "$HEADER" > "$CSV_FILE"


# --- Main Test Loop ---
# Loop through each parameter to be tested.
for param in "${PARAMETERS[@]}"; do
    
    # Print the current test parameter to the console for progress.
    echo "Running test for Parameter = ${param}..."
    
    # Create a temporary file to store the perf stat output.
    TEMP_PERF_FILE=$(mktemp)

    # Execute the command.
    # - perf's statistics (stderr) are redirected to the temporary file via the -o flag.
    # - The executable's standard output is captured into the EXECUTABLE_OUTPUT variable.
    EXECUTABLE_OUTPUT=$(perf stat -o "$TEMP_PERF_FILE" -e "$PERF_EVENTS" "$EXECUTABLE" "$param")
    
    # Read the perf statistics from the temporary file into a variable.
    PERF_OUTPUT=$(cat "$TEMP_PERF_FILE")
    
    # Clean up the temporary file.
    rm "$TEMP_PERF_FILE"

    # --- Parse the Perf Output ---
    # Start building the CSV row with the current parameter.
    CSV_ROW="${param}"

    # Use a loop to process each event name from the PERF_EVENTS variable.
    # IFS (Internal Field Separator) is temporarily set to a comma to split the string.
    IFS=',' read -ra EVENT_ARRAY <<< "$PERF_EVENTS"
    for event in "${EVENT_ARRAY[@]}"; do
        # Use awk to parse the output:
        # 1. Find the line that matches the current event name.
        # 2. Extract the first column, which is the numerical value.
        # 3. Remove commas from the number (e.g., 1,234,567 -> 1234567).
        # If the event is not found, 'value' will be empty.
        value=$(echo "$PERF_OUTPUT" | awk -v ev="$event" '$2 == ev {gsub(/,/, "", $1); print $1}')
        
        # If no value was found (e.g., "not counted"), use 0 as a placeholder.
        if [ -z "$value" ]; then
            value="0"
        fi
        
        # Append the extracted value to our CSV row.
        CSV_ROW="${CSV_ROW},${value}"
    done

    # --- Append Executable Output ---
    # Sanitize the output for CSV: replace newlines with a space and wrap in quotes
    # to handle potential commas or multi-line output.
    SANITIZED_OUTPUT=$(echo "$EXECUTABLE_OUTPUT" | tr '\n' ' ')
    CSV_ROW="${CSV_ROW},\"${SANITIZED_OUTPUT}\""

    # --- Log the Results ---
    # Append the completed CSV row to the file.
    echo "$CSV_ROW" >> "$CSV_FILE"

done

echo "----------------------------------------------------"
echo "All tests completed."
echo "Full results have been saved to '$CSV_FILE'."

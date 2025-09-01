#!/bin/bash
set -euo pipefail

# MSR IA32_MISC_ENABLE is at address 0x1a4
MSR_ADDR=0x1a4

# Bit 9: L2 Hardware Prefetcher, Bit 19: L2 Adj. Cache Line Prefetcher
DISABLE_MASK=$(((1<<9)|(1<<19)))

function show_status() {
    local current_val
    current_val=$(sudo rdmsr -c 0 "$MSR_ADDR")
    
    # Check if bit 9 OR bit 19 is set
    if (( ($current_val & $DISABLE_MASK) != 0 )); then
        echo "Hardware Prefetchers are currently DISABLED."
    else
        echo "Hardware Prefetchers are currently ENABLED."
    fi
    echo "Current MSR 0x$MSR_ADDR value: 0x$
    current_val"
}

function disable_prefetchers() {
    echo "Disabling hardware prefetchers..."
    local current_val
    current_val=$(sudo rdmsr -c 0 "$MSR_ADDR")
    
    local new_val
    new_val=$(( $current_val | $DISABLE_MASK ))
    
    sudo wrmsr -a "$MSR_ADDR" "0x$new_val"
    echo "Done."
    show_status
}

function enable_prefetchers() {
    echo "Enabling hardware prefetchers..."
    local current_val
    current_val=$(sudo rdmsr -c 0 "$MSR_ADDR")

    local new_val
    new_val=$(( $current_val & ~$DISABLE_MASK ))

    sudo wrmsr -a "$MSR_ADDR" "0x$new_val"
    echo "Done."
    show_status
}

# --- Main Script Logic ---
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root (or with sudo)." 
   exit 1
fi

modprobe msr &>/dev/null

case "$1" in
    disable)
        disable_prefetchers
        ;;
    enable)
        enable_prefetchers
        ;;
    status)
        show_status
        ;;
    *)
        echo "Usage: sudo $0 {enable|disable|status}"
        exit 1
        ;;
esac
#!/bin/sh

if [ $# -ne 3 ]; then
    echo "Usage: $0 <PID>"
    exit 1
fi

PID=$1


PAGE_SIZE=4096 # Get the system page size (usually 4096 bytes)
PAGEMAP="/proc/$PID/pagemap"

if [ ! -f "$PAGEMAP" ]; then
    echo "Pagemap file not found for PID $PID"
    exit 1
fi

echo "Scanning pages for process $PID..."
echo "Page Size: $PAGE_SIZE bytes"

# Iterate through the virtual address space
VIRTUAL_ADDRESS=$2
MAX_VIRTUAL_ADDRESS=$3
RAM_PAGES=0
TOTAL_PAGES=0

VIRTUAL_ADDRESS_DEC=$(printf "%d" "$VIRTUAL_ADDRESS")

while [ "$VIRTUAL_ADDRESS_DEC" -lt $(printf "%d" "$MAX_VIRTUAL_ADDRESS")  ];do
    OFFSET=$((VIRTUAL_ADDRESS_DEC / PAGE_SIZE * 8)) # Offset for this page
    PAGE_ENTRY=$(dd if="$PAGEMAP" bs=8 skip=$((VIRTUAL_ADDRESS_DEC / PAGE_SIZE)) count=1 2>/dev/null | hexdump -e '8/1 "%02x"')

    # Stop if no more entries can be read
    if [ -z "$PAGE_ENTRY" ]; then
        break
    fi

    # Check if the page is present
    IS_PRESENT=$(( (0x$PAGE_ENTRY >> 63) & 1 ))
    if [ "$IS_PRESENT" -eq 1 ]; then
        RAM_PAGES=$((RAM_PAGES + 1))
        PAGE_ENTRY_CLEAN=${PAGE_ENTRY#0x}
        PFN=$(( 0x$PAGE_ENTRY & ((1 << 55) - 1) ))

        PHY_ADDRESS=$((PFN * PAGE_SIZE + (VIRTUAL_ADDRESS_DEC % PAGE_SIZE)))
        echo "Page in RAM: VA=0x$(printf "%x" $VIRTUAL_ADDRESS_DEC), PFN=$(printf '%016x' $PHY_ADDRESS)"
    fi

    TOTAL_PAGES=$((TOTAL_PAGES + 1))
    VIRTUAL_ADDRESS=$((VIRTUAL_ADDRESS_DEC + PAGE_SIZE))
done

echo "Total Pages: $TOTAL_PAGES"
echo "Pages in RAM: $RAM_PAGES"
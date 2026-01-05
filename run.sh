#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 3 ]]; then
    echo "Usage: ./run.sh <day-number> <part> <input-file>"
    echo "  <day-number>: Day number (e.g., 1, 2, 3)"
    echo "  <part>:       part1 or part2"
    echo "  <input-file>: Input file name (e.g., input.txt, test.txt)"
    echo ""
    echo "Example: ./run.sh 1 part1 input.txt"
    echo "Example: ./run.sh 2 part2 test.txt"
    exit 1
fi

DAY=$(printf "day%02d" "$1")
PART="$2"
FILE="$3"

TARGET="${DAY}-${PART}"

cmake --build --preset default --target "${TARGET}"

"./build/${DAY}/${TARGET}" "${DAY}/${FILE}"

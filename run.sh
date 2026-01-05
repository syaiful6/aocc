#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 2 ]]; then
    echo "Usage: ./run.sh <day-number> <input-file>"
    echo "Example: ./run.sh 6 input.txt"
    exit 1
fi

DAY=$(printf "day%02d" "$1")
FILE="$2"

cmake --build --preset default --target "${DAY}"

"./build/${DAY}/${DAY}" "${DAY}/${FILE}"

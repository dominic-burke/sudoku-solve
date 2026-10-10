#!/bin/sh
set -eu

output_file=$(mktemp)
invalid_file=$(mktemp)
trap 'rm -f "$output_file" "$invalid_file"' EXIT HUP INT TERM

./build/sudoku_solve examples/sample_puzzle >"$output_file"
grep -Fq "Solution string: 316452978285679314497318562879234156142965783653187249968721435521843697734596821" "$output_file"

if ./build/sudoku_solve > /dev/null 2>&1; then
    echo "CLI accepted a missing puzzle path" >&2
    exit 1
fi

printf '123\n' >"$invalid_file"
if ./build/sudoku_solve "$invalid_file" > /dev/null 2>&1; then
    echo "CLI accepted a malformed puzzle" >&2
    exit 1
fi

echo "All Sudoku CLI tests passed."

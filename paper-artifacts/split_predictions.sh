#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "Usage: $0 INPUT.csv [OUTPUT_DIR]" >&2
    exit 1
fi

csv_file="$1"
out_dir="${2:-.}"

mkdir -p "$out_dir"

for q in 2 31 256; do
    for suffix in "" "-c" "-c-b"; do
        rm -f "$out_dir/XL-test-${q}${suffix}.txt"
    done
done

tmp="$(mktemp)"
trap 'rm -f "$tmp"' EXIT

# Sort by q, variant, n while keeping the header out.
{
    head -n 1 "$csv_file"
    tail -n +2 "$csv_file" | sort -t, -k1,1n -k5,5 -k2,2n
} > "$tmp"

awk -F, -v out_dir="$out_dir" '
NR == 1 {
    for (i = 1; i <= NF; i++) {
        gsub(/\r/, "", $i)
        col[$i] = i
    }
    next
}

{
    q = $(col["q"])
    variant = $(col["variant"])
    pred = $(col["pred_bit_ops"])

    gsub(/\r/, "", q)
    gsub(/\r/, "", variant)
    gsub(/\r/, "", pred)

    if (variant == "base" || variant == "baseline") {
        suffix = ""
    } else if (variant == "const") {
        suffix = "-c"
    } else if (variant == "bucket" || variant == "const_bucket") {
        suffix = "-c-b"
    } else {
        next
    }

    if (q == "2" || q == "31" || q == "256") {
        print pred >> out_dir "/XL-test-" q suffix ".txt"
    }
}
' "$tmp"


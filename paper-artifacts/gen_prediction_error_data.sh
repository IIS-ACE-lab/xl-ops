#!/usr/bin/env bash
set -euo pipefail

# Generate wrapper logs used by scripts/analyze_prediction_errors.py.
#
# Default output directory matches the public-repo Makefile target:
#   data/raw/prediction-errors
#
# Filenames keep the historical format expected by analyze_prediction_errors.py:
#   XL_test-<seed>-<q>-<nn>[-c|-c-b].txt

SAGE_BIN=${SAGE_BIN:-sage}
XLCOST=${XLCOST:-XL_cost.sage}
XLTEST=${XLTEST:-../src/bin/XL_test}
OUT_DIR=${OUT_DIR:-prediction-errors}
JOBS=${JOBS:-90%}
REPEATS=${REPEATS:-100}
M_FACTOR=${M_FACTOR:-2}

Q2_N_MIN=${Q2_N_MIN:-10}
Q2_N_MAX=${Q2_N_MAX:-15}
Q31_256_N_MIN=${Q31_256_N_MIN:-4}
Q31_256_N_MAX=${Q31_256_N_MAX:-9}

USE_GF2_OPT=${USE_GF2_OPT:-1}

usage() {
  cat <<USAGE
Usage: $0 [options]

Options:
  --out-dir DIR          output directory [default: $OUT_DIR]
  --sage CMD             Sage command [default: $SAGE_BIN]
  --compare FILE         xl_cost_compare.sage path [default: $XLCOST]
  --xl-test FILE         XL_test executable [default: $XLTEST]
  --jobs N               GNU parallel jobs [default: $JOBS]
  --repeats N            number of seeds/repeats [default: $REPEATS]
  --m-factor N           use m=N*n [default: $M_FACTOR]
  --q2-n-min N           first n for GF(2) [default: $Q2_N_MIN]
  --q2-n-max N           last n for GF(2) [default: $Q2_N_MAX]
  --q31-256-n-min N      first n for GF(31), GF(256) [default: $Q31_256_N_MIN]
  --q31-256-n-max N      last n for GF(31), GF(256) [default: $Q31_256_N_MAX]
  -h, --help             show this help

Environment variables with the same uppercase names can also be used.
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --out-dir) OUT_DIR=$2; shift 2 ;;
    --sage) SAGE_BIN=$2; shift 2 ;;
    --compare) XLCOST=$2; shift 2 ;;
    --xl-test) XLTEST=$2; shift 2 ;;
    --jobs) JOBS=$2; shift 2 ;;
    --repeats) REPEATS=$2; shift 2 ;;
    --m-factor) M_FACTOR=$2; shift 2 ;;
    --q2-n-min) Q2_N_MIN=$2; shift 2 ;;
    --q2-n-max) Q2_N_MAX=$2; shift 2 ;;
    --q31-256-n-min) Q31_256_N_MIN=$2; shift 2 ;;
    --q31-256-n-max) Q31_256_N_MAX=$2; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; usage >&2; exit 2 ;;
  esac
done

mkdir -p "$OUT_DIR"

run_one() {
  local repeat="$1"
  local q="$2"
  local n="$3"
  local mode="$4"

  local hex seed num m out flags=()
  hex=$(printf "%04d%04d%04d%s" "$repeat" "$q" "$n" "$mode" | sha256sum | cut -c1-16)
  seed="0x$hex"
  printf -v num "%02d" "$n"
  m=$((M_FACTOR * n))

  case "$mode" in
    plain)
      out="$OUT_DIR/XL-test-$seed-$q-$num.txt"
      ;;
    plain-c)
      flags+=(-c)
      out="$OUT_DIR/XL-test-$seed-$q-$num-c.txt"
      ;;
    plain-c-b)
      flags+=(-c -b)
      out="$OUT_DIR/XL-test-$seed-$q-$num-c-b.txt"
      ;;
    *)
      echo "unknown mode: $mode" >&2
      return 2
      ;;
  esac

  echo "repeat=$repeat q=$q n=$n m=$m mode=$mode seed=$seed -> $out" >&2

  "$SAGE_BIN" "$XLCOST" \
    --exe "$XLTEST" \
    -q "$q" -n "$n" -m "$m" \
    --seed "$seed" \
    --wrapper \
    "${flags[@]}" \
    > "$out"
}

export -f run_one
export SAGE_BIN XLCOST XLTEST OUT_DIR M_FACTOR USE_GF2_OPT

make_jobs() {
  local repeat n q mode

  for repeat in $(seq 1 "$REPEATS"); do
    for n in $(seq "$Q2_N_MIN" "$Q2_N_MAX"); do
      q=2
      for mode in plain plain-c plain-c-b; do
        printf "%s %s %s %s\n" "$repeat" "$q" "$n" "$mode"
      done
    done

    for n in $(seq "$Q31_256_N_MIN" "$Q31_256_N_MAX"); do
      for q in 31 256; do
        for mode in plain plain-c plain-c-b; do
          printf "%s %s %s %s\n" "$repeat" "$q" "$n" "$mode"
        done
      done
    done
  done
}

if command -v parallel >/dev/null 2>&1; then
  make_jobs | parallel -j"$JOBS" --colsep ' ' run_one {1} {2} {3} {4}
else
  echo "GNU parallel not found; running sequentially." >&2
  while read -r repeat q n mode; do
    run_one "$repeat" "$q" "$n" "$mode"
  done < <(make_jobs)
fi

# Stamp file for Makefile integration.
date -u +%Y-%m-%dT%H:%M:%SZ > "$OUT_DIR/.generated"

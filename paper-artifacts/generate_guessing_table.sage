#!/usr/bin/env sage
# vim: syntax=python

import argparse
import contextlib
import io
import os
import re
import sys
from math import log2


BITOPS_RE = re.compile(
    r"^bit ops:\s+\d+\s+\(pred\s+([0-9]+)\)",
    re.MULTILINE,
)

DEGREE_RE = re.compile(
    r"^q:\s*\d+\s+n:\s*\d+\s+m:\s*\d+\s+D:\s*(\d+)",
    re.MULTILINE,
)


# --------------------------------------------------------------------
# NIST additional-signature / MQOM-UOV-QR-UOV table data
# --------------------------------------------------------------------
#
# scheme, q, displayed n, displayed m, published WXL

TABLE_ROWS = [
    ("MQOM2-L1-gf256", 256, 48, 48, 157.0),
    ("MQOM2-L3-gf256", 256, 72, 72, 217.5),
    ("MQOM2-L5-gf256", 256, 96, 96, 280.4),

    ("uov-Ip",   256, 112, 44, 145),
    ("uov-III",  256, 184, 72, 218),
    ("uov-V",    256, 244, 96, 278),

    ("QR-UOV I (31, 165, 60, 3)",   31, 75, 60, 163),
    ("QR-UOV III (31, 246, 87, 3)", 31, 111, 87, 226),
    ("QR-UOV V (31, 324, 114, 3)",  31, 156, 114, 286),
]


# --------------------------------------------------------------------
# Fukuoka challenge table data
# --------------------------------------------------------------------
#
# type_label, type_tex, q, nrange, m_rule
# For each type, nrange has exactly four entries:
#   first two: largest broken challenges
#   last two:  largest provided challenges

LEFT_TYPES = [
    (
        "Type I",
        r"\textbf{Type I}  \\$\mathrm{GF}(2)$\\$m=2n$",
        {"q": 2, "nrange": [80, 83, 99, 100], "m_rule": "2n"},
    ),
    (
        "Type II",
        r"\textbf{Type II} \\$\mathrm{GF}(256)$\\$m=2n$",
        {"q": 256, "nrange": [36, 37, 53, 54], "m_rule": "2n"},
    ),
    (
        "Type III",
        r"\textbf{Type III}\\$\mathrm{GF}(31)$\\$m=2n$",
        {"q": 31, "nrange": [37, 38, 52, 53], "m_rule": "2n"},
    ),
]

RIGHT_TYPES = [
    (
        "Type IV",
        r"\textbf{Type IV}\\$\mathrm{GF}(2)$\\$n\approx1.5m$",
        {"q": 2, "nrange": [112, 114, 148, 150], "m_rule": "n1p5m"},
    ),
    (
        "Type V",
        r"\textbf{Type V} \\$\mathrm{GF}(256)$\\$n\approx1.5m$",
        {"q": 256, "nrange": [28, 30, 51, 52], "m_rule": "n1p5m"},
    ),
    (
        "Type VI",
        r"\textbf{Type VI}\\$\mathrm{GF}(31)$\\$n\approx1.5m$",
        {"q": 31, "nrange": [34, 36, 51, 52], "m_rule": "n1p5m"},
    ),
]


VARIANTS = [
    ("base", [], 0),
    ("c", ["-c"], 1),
    ("c+b", ["-c", "-b"], 2),
]


# --------------------------------------------------------------------
# Loading XL_cost.sage as a library
# --------------------------------------------------------------------

def load_XL_cost(XL_cost):
    XL_cost = os.path.abspath(XL_cost)

    globals()["XL_COST_FILE"] = XL_cost
    globals()["XL_COST_LIBRARY_MODE"] = True

    try:
        load(XL_cost)
    finally:
        globals()["XL_COST_LIBRARY_MODE"] = False


# --------------------------------------------------------------------
# Calling XL_cost.sage in the same Sage process
# --------------------------------------------------------------------

def call_xl_cost_pred(exe, q, n, m, flags):
    """
    Call XL_cost.sage main() in the current Sage process.

    This preserves the original XL_cost.sage logic, including:
      - running XL_test --f-cost in --pred mode,
      - parsing D from XL_test,
      - selecting formulas,
      - computing predicted bit-operation counts.

    Returns:
      {
        "pred_cost": integer predicted bit-operation count,
        "D": operating degree parsed from XL_test --f-cost output,
      }
    """
    argv = [
        "XL_cost.sage",
        "--exe", exe,
        "-q", str(q),
        "-n", str(n),
        "-m", str(m),
        "--pred",
    ]

    argv.extend(flags)

    old_argv = sys.argv[:]
    buf = io.StringIO()

    try:
        sys.argv = argv
        with contextlib.redirect_stdout(buf):
            try:
                main()
            except SystemExit as e:
                if e.code not in (0, None):
                    raise
    finally:
        sys.argv = old_argv

    out = buf.getvalue()

    m_bitops = BITOPS_RE.search(out)
    if not m_bitops:
        raise RuntimeError(
            "Could not parse predicted bit ops from XL_cost output.\n"
            f"argv: {' '.join(argv)}\n"
            f"output:\n{out}"
        )

    m_degree = DEGREE_RE.search(out)
    if not m_degree:
        raise RuntimeError(
            "Could not parse D from XL_cost output.\n"
            f"argv: {' '.join(argv)}\n"
            f"output:\n{out}"
        )

    return {
        "pred_cost": int(m_bitops.group(1)),
        "D": int(m_degree.group(1)),
    }


def print_best_sanity(label, bests, names):
  print(label, file=sys.stderr)
  for name, best in zip(names, bests):
      n_red = best.get("n_red", best.get("n"))
      m_red = best.get("m_red", best.get("m"))
      print(
          f"  {name:>4}: "
          f"n'={n_red}, m'={m_red}, D={best['D']}, "
          f"k={best['k']}, "
          f"log2={best['log2_total']:.4f}",
          file=sys.stderr,
      )


# --------------------------------------------------------------------
# Guessing search
# --------------------------------------------------------------------

def m_from_rule(n, rule):
    if rule == "2n":
        return 2 * n

    if rule == "n1p5m":
        # n approx 1.5 m => m approx 2n/3.
        return int(round(2 * n / 3))

    raise ValueError(f"unknown rule: {rule}")


def find_best_square_style(exe, q, m, flags, min_n=10):
    """
    Search the old square-style strategy used by the MQOM/UOV/QR-UOV table.

    The reduced system has m equations and n=m-k variables. The total cost is

        q^k * C_q(n,m).

    This matches the behavior of the original find_best_guessing.py script.
    """
    best = None

    for k in range(2, m - min_n + 1):
        n = m - k

        pred = call_xl_cost_pred(
            exe=exe,
            q=q,
            n=n,
            m=m,
            flags=flags,
        )

        pred_cost = pred["pred_cost"]
        D_value = pred["D"]
        log2_total = log2(pred_cost) + k * log2(q)

        record = {
            "n": n,
            "m": m,
            "D": D_value,
            "k": k,
            "pred_cost": pred_cost,
            "log2_total": log2_total,
        }

        if best is None or record["log2_total"] < best["log2_total"]:
            best = record

    if best is None:
        raise RuntimeError(f"No valid guessing point found for q={q}, m={m}")

    return best


def find_best_from_original_system(exe, q, n0, m0, flags, min_n=10):
    """
    Optimize over variable guessing for an original system with n0 variables
    and m0 equations.

    After guessing g variables, the reduced XL system has

        n' = n0 - g,   m' = m0.

    Guesses up to the square-system threshold n'=m0 do not incur an
    exhaustive-search penalty in the underdetermined case. Additional guesses
    contribute a factor q^k, where

        k = max(0, g - max(0, n0-m0)).

    We only consider reduced systems satisfying m' >= n' + 2, matching the
    Wiedemann-XL regime used elsewhere in the paper. This means that for an
    initially underdetermined system, at least two guesses beyond the square
    threshold are required.
    """
    best = None
    max_g = n0 - min_n
    free_guesses = max(0, n0 - m0)

    for g in range(0, max_g + 1):
        n_red = n0 - g
        m_red = m0

        if m_red < n_red + 2:
            continue

        k_penalty = max(0, g - free_guesses)

        try:
            pred = call_xl_cost_pred(
                exe=exe,
                q=q,
                n=n_red,
                m=m_red,
                flags=flags,
            )
        except Exception:
            continue

        pred_cost = pred["pred_cost"]
        D_value = pred["D"]
        log2_total = log2(pred_cost) + k_penalty * log2(q)

        record = {
            "n_red": n_red,
            "m_red": m_red,
            "D": D_value,
            "g": g,
            "k": k_penalty,
            "pred_cost": pred_cost,
            "log2_total": log2_total,
        }

        if best is None or record["log2_total"] < best["log2_total"]:
            best = record

    if best is None:
        raise RuntimeError(
            f"No valid guessing point found for q={q}, n={n0}, m={m0}"
        )

    return best


# --------------------------------------------------------------------
# Formatting helpers
# --------------------------------------------------------------------

def fmt_wxl(x):
    if isinstance(x, int):
        return str(x)
    if abs(x - round(x)) < 1e-9:
        return str(int(round(x)))
    return f"{x:.1f}"


def row_group_for_nist_table(scheme):
    if scheme.startswith("MQOM2"):
        return "MQOM2"
    if scheme.startswith("uov"):
        return "uov"
    if scheme.startswith("QR-UOV"):
        return "QR-UOV"
    return "other"


def lg_fmt(x, digits=2):
    return f"{x:.{digits}f}"


# --------------------------------------------------------------------
# LaTeX output: MQOM/UOV/QR-UOV guessing table
# --------------------------------------------------------------------

def write_latex_table(rows, exe, out="-", min_n=10):
    """
    Generate the MQOM/UOV/QR-UOV comparison table.

    This keeps the behavior of the original script:
    for each row, use m from the table and search n=m-k.
    """
    lines = []

    lines.append(r"\begin{tabular}{@{}lrrr@{\hspace*{1.5em}}r@{\hspace*{1.5em}}rrrrr@{}}")
    lines.append(r"\toprule")
    lines.append(
        r"Scheme & $q$ & $n$ & $m$ & WXL "
        r"& base & const & c+b "
        r"& $D$ & $k$ \\"
    )
    lines.append(r"\midrule")

    prev_group = None

    for scheme, q, table_n, m, wxl in rows:
        group = row_group_for_nist_table(scheme)
        if prev_group is not None and group != prev_group:
            lines.append(r"\midrule")

        bests = []
        for _name, flags, _idx in VARIANTS:
            bests.append(
                find_best_square_style(
                    exe=exe,
                    q=q,
                    m=m,
                    flags=flags,
                    min_n=min_n,
                )
            )

        print_best_sanity(
            label=f"{scheme}: q={q}, displayed n={table_n}, m={m}",
            bests=bests,
            names=["base", "c", "c+b"],
        )

        # Show k for the const+bucket minimizer, matching the c+b column.
        k_value = bests[2]["k"]
        D_value = bests[2]["D"]

        lines.append(
            rf"{scheme} & {q} & {table_n} & {m} & {fmt_wxl(wxl)} "
            rf"& {bests[0]['log2_total']:.1f} "
            rf"& {bests[1]['log2_total']:.1f} "
            rf"& {bests[2]['log2_total']:.1f} "
            rf"& {D_value} "
            rf"& {k_value} \\"
        )

        prev_group = group

    lines.append(r"\bottomrule")
    lines.append(r"\end{tabular}")

    text = "\n".join(lines)

    if out == "-":
        print(text)
    else:
        with open(out, "w") as f:
            f.write(text + "\n")


# --------------------------------------------------------------------
# LaTeX output: Fukuoka optimized-guessing table, old two-block layout
# --------------------------------------------------------------------

SIDE_DATA_COLS = 5
SIDE_TOTAL_COLS = 1 + SIDE_DATA_COLS


def fukuoka_section_row(label):
    return rf"\multicolumn{{{SIDE_DATA_COLS}}}{{c}}{{\emph{{{label}}}}}"


def compute_fukuoka_entry(q, n0, m0, exe, min_n=10):
    """
    Compute the baseline and const+bucket optimized guessing entries for one
    original Fukuoka parameter set.

    The displayed D and k are those of the minimizing const+bucket point.
    """
    best_base = find_best_from_original_system(
        exe=exe,
        q=q,
        n0=n0,
        m0=m0,
        flags=[],
        min_n=min_n,
    )

    best_bucket = find_best_from_original_system(
        exe=exe,
        q=q,
        n0=n0,
        m0=m0,
        flags=["-c", "-b"],
        min_n=min_n,
    )

    print_best_sanity(
        label=f"q={q}, n={n0}, m={m0}",
        bests=[best_base, best_bucket],
        names=["base", "c+b"],
    )

    return {
        "n": n0,
        "m": m0,
        "D": best_bucket["D"],
        "k": best_bucket["k"],
        "log2_base": best_base["log2_total"],
        "log2_cb": best_bucket["log2_total"],
    }


def collect_fukuoka_block(type_tex, cfg, exe, min_n=10):
    q = cfg["q"]
    rows = []

    for idx, n in enumerate(cfg["nrange"]):
        m = m_from_rule(n, cfg["m_rule"])
        entry = compute_fukuoka_entry(
            q=q,
            n0=n,
            m0=m,
            exe=exe,
            min_n=min_n,
        )
        entry["set"] = "broken" if idx < 2 else "provided"
        rows.append(entry)

    return {
        "type_tex": type_tex,
        "rows": rows,
    }


def render_fukuoka_block_lines(block, side):
    """
    Render one side block as six rows:
      section row for largest broken challenges,
      two data rows,
      section row for largest provided challenges,
      two data rows.

    The type cell is a multirow spanning all six rows, matching the old layout
    style while adding D, k, and section labels.
    """
    type_cell = rf"\multirow{{6}}{{*}}{{\shortstack{{{block['type_tex']}}}}}"
    out = []

    def data_cols(entry):
        if side == "left":
          nm = rf"${entry['n']}$ & "
        else:
          nm = rf"${entry['m']}$ & "

        return (
            nm +
            rf"${entry['D']}$ & "
            rf"${entry['k']}$ & "
            rf"${lg_fmt(entry['log2_base'])}$ & "
            rf"${lg_fmt(entry['log2_cb'])}$"
        )

    rows = block["rows"]

    out.append((type_cell, fukuoka_section_row("largest broken challenges")))
    out.append(("", data_cols(rows[0])))
    out.append(("", data_cols(rows[1])))
    out.append(("", fukuoka_section_row("largest provided challenges")))
    out.append(("", data_cols(rows[2])))
    out.append(("", data_cols(rows[3])))

    return out


def write_fukuoka_guessing_table(left_types, right_types, exe, out="-", min_n=10):
    """
    Generate the Fukuoka table in the old two-column layout, but with:
      - optimized guessing,
      - D from XL_test/XL_cost,
      - k for the minimizing const+bucket point,
      - explicit largest-broken/largest-provided section rows.
    """
    lines = []

    lines.append(r"\begin{tabular}{@{}lrrrrr@{\hspace*{0.8cm}}lrrrrr@{}}")
    lines.append(r"\toprule")
    lines.append(
        r"Type & $n$ & $D$ & $k$ & base & c+b & "
        r"Type & $m$ & $D$ & $k$ & base & c+b \\"
    )
    lines.append(r"\toprule")

    pairs = list(zip(left_types, right_types))

    for idx, ((_lname, ltex, lcfg), (_rname, rtex, rcfg)) in enumerate(pairs):
        left_block = collect_fukuoka_block(
            ltex,
            lcfg,
            exe=exe,
            min_n=min_n,
        )
        right_block = collect_fukuoka_block(
            rtex,
            rcfg,
            exe=exe,
            min_n=min_n,
        )

        left_lines = render_fukuoka_block_lines(left_block, "left")
        right_lines = render_fukuoka_block_lines(right_block, "right")

        for (ltype, lcols), (rtype, rcols) in zip(left_lines, right_lines):
            lines.append(
                rf"{ltype} & {lcols} & {rtype} & {rcols} \\"
            )

        if idx + 1 != len(pairs):
            lines.append(r"\midrule")

    lines.append(r"\bottomrule")
    lines.append(r"\end{tabular}")

    text = "\n".join(lines)

    if out == "-":
        print(text)
    else:
        with open(out, "w") as f:
            f.write(text + "\n")


# --------------------------------------------------------------------
# Main
# --------------------------------------------------------------------

def main_driver():
    ap = argparse.ArgumentParser(
        description=(
            "Generate guessing-comparison tables using XL_cost.sage "
            "inside one Sage session."
        )
    )

    ap.add_argument(
        "--XL_cost",
        default="../XL_cost.sage",
        help="Path to XL_cost.sage.",
    )

    ap.add_argument(
        "--exe",
        default="../src/bin/XL_test",
        help="Path to XL_test executable.",
    )

    ap.add_argument(
        "--guessing-output",
        default="-",
        help=(
            "Output file for the MQOM/UOV/QR-UOV guessing table, "
            "or '-' for stdout. Use 'none' to disable."
        ),
    )

    ap.add_argument(
        "--fukuoka-output",
        default=None,
        help=(
            "Output file for the Fukuoka optimized-guessing table. "
            "If omitted, this table is not generated. Use '-' for stdout."
        ),
    )

    ap.add_argument(
        "--min-n",
        type=int,
        default=10,
        help="Minimum remaining variable count.",
    )

    args = ap.parse_args()

    load_XL_cost(args.XL_cost)

    if args.guessing_output != "none":
        write_latex_table(
            TABLE_ROWS,
            exe=args.exe,
            out=args.guessing_output,
            min_n=args.min_n,
        )

    if args.fukuoka_output is not None:
        write_fukuoka_guessing_table(
            LEFT_TYPES,
            RIGHT_TYPES,
            exe=args.exe,
            out=args.fukuoka_output,
            min_n=args.min_n,
        )


if __name__ == "__main__":
    main_driver()

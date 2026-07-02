#!/usr/bin/env sage
# vim: syntax=python

import argparse
import contextlib
import io
import os
import re
import sys
from math import log2


BITOPS_RE = re.compile(r"^bit ops:\s+\d+\s+\(pred\s+([0-9]+)\)", re.MULTILINE)


TABLE_ROWS = [
    # scheme, q, table_n, m, WXL
    ("MQOM2-L1-gf256", 256, 48, 48, 157.0),
    ("MQOM2-L3-gf256", 256, 72, 72, 217.5),
    ("MQOM2-L5-gf256", 256, 96, 96, 280.4),

    ("uov-Ip",  256, 112, 44, 145),
    ("uov-III", 256, 184, 72, 218),
    ("uov-V",   256, 244, 96, 278),

    ("QR-UOV I (31, 165, 60, 3)",    31, 75, 60, 163),
    ("QR-UOV III (31, 246, 87, 3)",  31, 111, 87, 226),
    ("QR-UOV V (31, 324, 114, 3)",   31, 156, 114, 286),
]


VARIANTS = [
    ("base", [], 0),
    ("c", ["-c"], 1),
    ("c+b", ["-c", "-b"], 2),
]


def load_compare_file(compare_file):
    compare_file = os.path.abspath(compare_file)

    globals()["XL_COST_COMPARE_FILE"] = compare_file
    globals()["XL_COST_COMPARE_LIBRARY_MODE"] = True

    try:
        load(compare_file)
    finally:
        globals()["XL_COST_COMPARE_LIBRARY_MODE"] = False


def call_xl_cost_compare_pred(exe, q, n, m, flags):
    """
    Call xl_cost_compare.sage main() in the current Sage process.

    This preserves the original xl_cost_compare.sage logic, including:
      - running XL-test --f-cost in --pred mode,
      - parsing D from XL-test,
      - selecting the formulas,
      - computing the predicted bit-operation count.
    """
    argv = [
        "xl_cost_compare.sage",
        "--exe", exe,
        "-q", str(q),
        "-n", str(n),
        "-m", str(m),
        "--pred",
    ]

    if q == 2:
        argv.append("--GF2-opt")

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
            "Could not parse predicted bit ops from xl_cost_compare output.\n"
            f"argv: {' '.join(argv)}\n"
            f"output:\n{out}"
        )

    return int(m_bitops.group(1))


def find_best(exe, q, m, flags, min_n=10):
    best = None

    for k in range(2, m - min_n + 1):
        n = m - k

        pred_cost = call_xl_cost_compare_pred(
            exe=exe,
            q=q,
            n=n,
            m=m,
            flags=flags,
        )

        log2_total = log2(pred_cost) + k * log2(q)

        record = {
            "n": n,
            "k": k,
            "pred_cost": pred_cost,
            "log2_total": log2_total,
        }

        if best is None or record["log2_total"] < best["log2_total"]:
            best = record

    return best


def fmt_wxl(x):
    if isinstance(x, int):
        return str(x)
    if abs(x - round(x)) < 1e-9:
        return str(int(round(x)))
    return f"{x:.1f}"


def row_group(scheme):
    if scheme.startswith("MQOM2"):
        return "MQOM2"
    if scheme.startswith("uov"):
        return "uov"
    if scheme.startswith("QR-UOV"):
        return "QR-UOV"
    return "other"


def write_latex_table(rows, exe, out="-", min_n=10):
    lines = []

    lines.append(r"\begin{tabular}{@{}lrrrlrlrrrr@{}}")
    lines.append(r"\toprule")
    lines.append(r" \multicolumn{1}{@{}c}{Scheme}")
    lines.append(r" & \multicolumn{1}{c}{\(q\)}")
    lines.append(r" & \multicolumn{1}{c}{\(n\)}")
    lines.append(r" & \multicolumn{1}{c}{\(m\)}")
    lines.append(r" &")
    lines.append(r" & \multicolumn{1}{c}{WXL}")
    lines.append(r" &")
    lines.append(r" & \multicolumn{1}{c}{\(C_{\mathrm{base}}\)}")
    lines.append(r" & \multicolumn{1}{c}{\(C_{\mathrm{c}}\)}")
    lines.append(r" & \multicolumn{1}{c}{\(C_{\mathrm{c+b}}\)}")
    lines.append(r" & \multicolumn{1}{c@{}}{\(k\)} \\")
    lines.append(r"\midrule")

    prev_group = None

    for scheme, q, table_n, m, wxl in rows:
        group = row_group(scheme)
        if prev_group is not None and group != prev_group:
            lines.append(r"\midrule")

        bests = []

        for _name, flags, _idx in VARIANTS:
            bests.append(
                find_best(
                    exe=exe,
                    q=q,
                    m=m,
                    flags=flags,
                    min_n=min_n,
                )
            )

        # The screenshot's single k column appears to correspond to c+b.
        k_value = bests[2]["k"]

        lines.append(
            rf"{scheme} & {q} & {table_n} & {m} & & {fmt_wxl(wxl)} & "
            rf"& {bests[0]['log2_total']:.1f} "
            rf"& {bests[1]['log2_total']:.1f} "
            rf"& {bests[2]['log2_total']:.1f} "
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


def main_driver():
    ap = argparse.ArgumentParser(
        description="Generate guessing comparison table using xl_cost_compare.sage in one Sage session."
    )

    ap.add_argument(
        "--compare-file",
        default="sage/xl_cost_compare.sage",
        help="Path to xl_cost_compare.sage.",
    )

    ap.add_argument(
        "--exe",
        default="src/bin/XL-test",
        help="Path to XL-test executable.",
    )

    ap.add_argument(
        "--output",
        default="-",
        help="Output LaTeX file, or '-' for stdout.",
    )

    ap.add_argument(
        "--min-n",
        type=int,
        default=10,
        help="Minimum remaining variable count n=m-k.",
    )

    args = ap.parse_args()

    load_compare_file(args.compare_file)

    write_latex_table(
        TABLE_ROWS,
        exe=args.exe,
        out=args.output,
        min_n=args.min_n,
    )


if __name__ == "__main__":
    main_driver()


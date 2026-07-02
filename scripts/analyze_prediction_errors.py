#!/usr/bin/env python3

import math
import re
import argparse
import glob
from collections import defaultdict
from pathlib import Path


BITOPS_RE = re.compile(r"^bit ops: ([0-9]+) \(pred ([0-9]+)\)")
DEGREE_RE = re.compile(r"D:\s*([0-9]+)")

VARIANT_ORDER = {
    "base": 0,
    "c": 1,
    "c-b": 2,
}


def mean(xs):
    return sum(xs) / len(xs)


def sample_std(xs):
    if len(xs) <= 1:
        return 0.0
    m = mean(xs)
    return math.sqrt(sum((x - m) ** 2 for x in xs) / (len(xs) - 1))


def ci95(xs):
    # two-sided 95% CI, df = 9
    t_crit = 2.262
    return t_crit * sample_std(xs) / math.sqrt(len(xs))


def classify(name):
    stem = name.removesuffix(".txt")
    parts = stem.split("-")

    field = parts[0]

    n = int(parts[1])

    if field == "2":
        field_label = r"$\mathrm{GF}(2)$"
    elif field == "31":
        field_label = r"$\mathrm{GF}(31)$"
    elif field == "256":
        field_label = r"$\mathrm{GF}(256)$"
    else:
        field_label = rf"$\mathrm{{GF}}({field})$"

    if stem.endswith("-c-b"):
        variant = "const+bucket"
    elif stem.endswith("-c"):
        variant = "const"
    elif "-opt" in stem:
        variant = "baseline"
    else:
        variant = "baseline"

    return field, field_label, variant, n


def sort_key(r):
    stem = r["name"].removesuffix(".txt")
    parts = stem.split("-")

    q = int(parts[0])
    n = int(parts[1])

    # remove "opt"
    variant_parts = [p for p in parts[2:] if p != "opt"]
    variant = "-".join(variant_parts) if variant_parts else "base"

    return (
        VARIANT_ORDER.get(variant, 99),  # variant first
        q,                                # then field
        n,                                # then size
    )


def case_label(r):
    stem = r["name"].removesuffix(".txt")
    parts = stem.split("-")

    n = parts[1]

    D = r.get("D")

    # remove "opt" from variant parts

    variant_parts = [p for p in parts[2:] if p != "opt"]
    variant = "-".join(variant_parts) if variant_parts else "base"

    return rf"{{{variant}, $n={int(n)}, D={D}$}}"

def case_label_with_field(r):
    stem = r["name"].removesuffix(".txt")
    parts = stem.split("-")

    q = parts[0]
    n = parts[1]

    D = r.get("D")

    variant_parts = [p for p in parts[2:] if p != "opt"]
    variant = "-".join(variant_parts) if variant_parts else "base"

    return rf"{{{variant}, $q={q}, n={int(n)}, D={D}$}}"


def parse_logs(input_dir="data-comp"):
    pattern = str(Path(input_dir) / "*.txt")
    rows = defaultdict(list)

    for path in glob.glob(pattern):
        path = Path(path)

        # Matches avg.sh behavior: remove prefix through XL-test-<seed>-
        m = re.search(r"XL-test-(?:0x)?[0-9a-fA-F]+-(.+)$", path.name)
        if not m:
            raise ValueError(f"Unexpected filename: {path.name}")
        
        key = m.group(1)

        D_value = None

        for line in path.read_text(errors="replace").splitlines():
            m = BITOPS_RE.match(line.strip())
            if m:
                actual = int(m.group(1))
                pred = int(m.group(2))
                rel = (pred - actual) / actual

                rows[key].append(
                    {
                        "actual": actual,
                        "pred": pred,
                        "rel": rel,
                        "abs_rel": abs(rel),
                        "D": D_value,
                    }
                )

            m = DEGREE_RE.search(line)
            if m:
                D_value = int(m.group(1))

    return rows


def summarize(rows):
    summary = []

    for name, vals in sorted(rows.items()):
        actuals = [v["actual"] for v in vals]
        preds = [v["pred"] for v in vals]
        rels = [v["rel"] for v in vals]
        abs_rels = [v["abs_rel"] for v in vals]

        Ds = [v["D"] for v in vals if v["D"] is not None]
        D = Ds[0] if Ds else None

        field, field_label, variant, n = classify(name)

        summary.append(
            {
                "name": name,
                "field": field,
                "field_label": field_label,
                "variant": variant,
                "n": n,
                "num": len(vals),
                "mean_actual": mean(actuals),
                "mean_pred": mean(preds),
                "mean_rel": mean(rels),
                "min_rel": min(rels),
                "max_rel": max(rels),
                "std_rel": sample_std(rels),
                "ci95_rel": ci95(rels),
                "mean_abs_rel": mean(abs_rels),
                "max_abs_rel": max(abs_rels),
                 "D": D,
            }
        )

    return summary


def write_latex_table(summary, out="prediction_error_table.tex"):
    grouped = defaultdict(list)

    for r in summary:
        grouped[(r["field"], r["field_label"], r["variant"])].append(r)

    lines = []
    lines.append(r"\begin{tabular*}{\linewidth}{@{}l@{\extracolsep{\fill}}lccr@{\extracolsep{1ex}}r@{\extracolsep{\fill}}r@{}}")
    lines.append(r"\toprule")
    lines.append(r"\multicolumn{1}{c}{Field} & \multicolumn{1}{c}{Variant} & \multicolumn{1}{c}{Cases: $n$} & Runs/case & \multicolumn{2}{c}{Mean rel. err.} & \multicolumn{1}{c}{Range} \\")
    lines.append(r"\toprule")

    prev_field = None

    for (field, field_label, variant), rs in sorted(grouped.items(), key=lambda x: (int(x[0][0]), x[0][2])):
        if prev_field is not None and field != prev_field:
            lines.append(r"\midrule")

        mean_rel = mean([r["mean_rel"] for r in rs])
        min_rel = min([r["min_rel"] for r in rs])
        max_rel = max([r["max_rel"] for r in rs])
        std_rel = sample_std([r["mean_rel"] for r in rs])
        max_abs = max(r["max_abs_rel"] for r in rs)
        runs = rs[0]["num"]

        if (rf"{100 * min_rel:.3f}" != rf"{100 * max_rel:.3f}"):
           minmax = rf"${100 * min_rel:.3f}\%$ --- ${100 * max_rel:.3f}\%$"
        else:
           minmax = r"\multicolumn{1}{c}{" + rf"${100 * min_rel:.3f}\%$" + r"}"

        lines.append(
            rf"{field_label} & {variant} & {min([r['n'] for r in rs])}--{max([r['n'] for r in rs])}& {runs} & "
            rf"${100 * mean_rel:.3f}$ & $(\pm {100 * std_rel:.3f}\%)$ & "
            + minmax + r"\\"
            #rf"${100 * max_abs:.3f}\%$ \\"
        )

        prev_field = field


    lines.append(r"\bottomrule\\[-0.5ex]")
    lines.append(r"\end{tabular*}")

    Path(out).write_text("\n".join(lines))



def write_tikz(summary, out="prediction_error_plots.tex"):
    lines = []

    # ------------------------------------------------------------
    # Plot: split relative error by field
    # ------------------------------------------------------------
    fields = [
        ("2", r"$\mathrm{GF}(2)$"),
        ("31", r"$\mathrm{GF}(31)$"),
        ("256", r"$\mathrm{GF}(256)$"),
    ]

    for field, field_label in fields:
        field_rows = [r for r in summary if r["field"] == field]

        if not field_rows:
            continue

        lines.append(r"\begin{tikzpicture}")
        lines.append(r"\begin{axis}[")
        lines.append(r"width=0.9\linewidth,")
        lines.append(r"height=0.33\linewidth,")
        lines.append(rf"title={{{field_label}}},")
        lines.append(r"enlarge x limits={abs=0.5},")
        lines.append(r"ylabel={Rel. error (\%)},")

        labels = [case_label(r) for r in field_rows]
        
#        lines.append(r"xlabel={Configuration},")
        lines.append(r"xtick=data,")
        lines.append(r"xticklabels={")
        lines.append(",".join(labels))
        lines.append(r"},")
        lines.append(r"x tick label style={rotate=90, anchor=east, font=\scriptsize},")

        lines.append(r"grid=both,")
        lines.append(r"]")

        lines.append(r"\addplot+[")
        lines.append(r"only marks,")
        lines.append(r"mark=x,")
        lines.append(r"mark options={line width=0.8pt},")
        lines.append(r"error bars/.cd,")
        lines.append(r"y dir=both,")
        lines.append(r"y explicit,")
        lines.append(r"error bar style={red},")
        lines.append(r"error mark=|,")
        lines.append(r"error mark options={red, line width=0.8pt}")
        lines.append(r"] coordinates {")

        for i, r in enumerate(field_rows, start=1):
            y = 100 * r["mean_rel"]
            y_max = 100 * r["max_rel"]
            y_min = 100 * r["min_rel"]
            err = 100 * r["ci95_rel"]
            if err < 0.0000001:
              lines.append(f"({i},{y:.6f})")
            else:
              lines.append(f"({i},{y:.6f}) +- (0,{err:.6f})")

        lines.append(r"};")

        lines.append(
            rf"\addplot[no marks, dashed] coordinates {{(1,0) ({len(field_rows)},0)}};"
        )

        lines.append(r"\end{axis}")
        lines.append(r"\end{tikzpicture}")
        lines.append(r"\par\medskip")

    Path(out).write_text("\n".join(lines))


def print_summary(summary):
    for r in summary:
        stem = r["name"].removesuffix(".txt")
        parts = stem.split("-")

        q = parts[0]
        n = int(parts[1])
        variant_parts = [p for p in parts[2:] if p != "opt"]
        variant = "-".join(variant_parts) if variant_parts else "base"

        print(
            f"name={r['name']:25s} "
            f"q={q:>3s} n={n:2d} variant={variant:4s} "
            f"mean_rel={100*r['mean_rel']: .4f}% "
            f"ci95={100*r['ci95_rel']: .4f}%"
        )


def main():
    ap = argparse.ArgumentParser(
        description=(
            "Analyze XL-test wrapper logs and generate a LaTeX summary table "
            "and PGF/TikZ plot snippet."
        )
    )
    ap.add_argument(
        "--input-dir",
        default="data-comp",
        help="Directory containing XL-test wrapper log .txt files.",
    )
    ap.add_argument(
        "--table-output",
        default="prediction_error_table.tex",
        help="Output path for the LaTeX table snippet.",
    )
    ap.add_argument(
        "--figure-output",
        default="prediction_error_plots.tex",
        help="Output path for the PGF/TikZ figure snippet.",
    )
    ap.add_argument(
        "--quiet",
        action="store_true",
        help="Do not print the textual summary to stdout.",
    )
    args = ap.parse_args()

    rows = parse_logs(args.input_dir)
    summary = sorted(summarize(rows), key=sort_key)

    if not args.quiet:
        print_summary(summary)

    Path(args.table_output).parent.mkdir(parents=True, exist_ok=True)
    Path(args.figure_output).parent.mkdir(parents=True, exist_ok=True)

    write_latex_table(summary, out=args.table_output)
    write_tikz(summary, out=args.figure_output)

if __name__ == "__main__":
    main()


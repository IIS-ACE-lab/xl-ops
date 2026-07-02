#!/usr/bin/env python3

from pathlib import Path
import argparse
import csv


FIELDS = ["2", "31", "256"]
VARIANTS = ["baseline", "const", "const_bucket"]

FIELD_LABELS = {
    "2": r"$\mathrm{GF}(2)$",
    "31": r"$\mathrm{GF}(31)$",
    "256": r"$\mathrm{GF}(256)$",
}

VARIANT_LABELS = {
    "baseline": "baseline",
    "const": "const",
    "const_bucket": "const+bucket",
}

# Keep the asymptotic estimates in the original whitespace-separated files.
DENOMINATORS = {
    "2": "get_D-2.txt",
    "31": "get_D-31.txt",
    "256": "get_D-256.txt",
}

# Accepted spellings in the CSV's variant column.
VARIANT_ALIASES = {
    "": "baseline",
    "baseline": "baseline",
    "base": "baseline",
    "opt": "baseline",
    "const": "const",
    "constant": "const",
    "bucket": "const_bucket",
    "const_bucket": "const_bucket",
    "const-bucket": "const_bucket",
    "const+bucket": "const_bucket",
    "c-b": "const_bucket",
}


class CsvFormatError(ValueError):
    pass


def normalize_q(q):
    q = str(q).strip()
    if q == "2-opt":
        return "2"
    return q


def normalize_variant(variant):
    key = str(variant).strip().lower().replace(" ", "_")
    if key not in VARIANT_ALIASES:
        raise CsvFormatError(
            f"Unknown variant {variant!r}; expected one of "
            f"{', '.join(sorted(k for k in VARIANT_ALIASES if k))}"
        )
    return VARIANT_ALIASES[key]


def parse_int_field(row, name, filename, row_number):
    value = row.get(name)
    if value is None or str(value).strip() == "":
        raise CsvFormatError(f"Missing {name!r} in {filename}, row {row_number}")

    try:
        return int(str(value).strip())
    except ValueError as exc:
        raise CsvFormatError(
            f"Expected integer {name!r} in {filename}, row {row_number}; got {value!r}"
        ) from exc


def read_prediction_costs_csv(filename):
    """Return {(field, variant): [(n, cost), ...]} from the combined CSV file."""
    required_columns = {"q", "n", "m", "D", "variant", "pred_bit_ops"}
    costs = {(field, variant): [] for field in FIELDS for variant in VARIANTS}
    seen = set()

    with open(filename, newline="") as f:
        reader = csv.DictReader(f)
        if reader.fieldnames is None:
            raise CsvFormatError(f"Empty CSV file: {filename}")

        missing = required_columns - set(reader.fieldnames)
        if missing:
            raise CsvFormatError(
                f"CSV file {filename} is missing column(s): {', '.join(sorted(missing))}"
            )

        for row_number, row in enumerate(reader, start=2):
            field = normalize_q(row["q"])
            if field not in FIELDS:
                continue

            variant = normalize_variant(row["variant"])
            n = parse_int_field(row, "n", filename, row_number)
            cost = parse_int_field(row, "pred_bit_ops", filename, row_number)

            key = (field, variant, n)
            if key in seen:
                raise CsvFormatError(
                    f"Duplicate CSV entry for q={field}, variant={variant}, n={n} "
                    f"in {filename}, row {row_number}"
                )
            seen.add(key)
            costs[(field, variant)].append((n, cost))

    for key in costs:
        costs[key].sort(key=lambda item: item[0])

    return costs


def read_big_o_costs(filename, start_n):
    costs = []
    with open(filename) as f:
        for row, line in enumerate(f, start=1):
            parts = line.split()
            if not parts:
                continue
            if len(parts) < 2:
                raise ValueError(
                    f"Expected at least two columns in {filename}, row {row}"
                )
            costs.append((start_n + len(costs), int(parts[1])))
    return costs


def quotients_from_costs(num_costs, den_costs):
    """Return quotient samples as (n, numerator / denominator)."""
    den_by_n = dict(den_costs)
    values = []

    for n, numerator in num_costs:
        if n not in den_by_n:
            raise ValueError(f"No asymptotic denominator available for n={n}")

        denominator = den_by_n[n]
        if denominator == 0:
            raise ZeroDivisionError(f"division by zero for n={n}")

        values.append((n, numerator / denominator))

    return values


def estimate_limit_n2(ns, y, tail_fraction=0.4):
    """
    Estimate L using y(n) = L + C n^(-2) on the tail.
    """
    m = len(y)
    if m < 2:
        raise ValueError("Need at least two quotient samples to estimate a limit")

    start = int((1.0 - tail_fraction) * m)
    start = max(0, min(start, m - 2))

    xs = [n ** -2 for n in ns[start:]]
    ys = y[start:]

    xbar = sum(xs) / len(xs)
    ybar = sum(ys) / len(ys)

    sxx = sum((x - xbar) ** 2 for x in xs)
    if sxx == 0:
        raise ValueError("Cannot estimate limit from identical n values")

    sxy = sum((x - xbar) * (yy - ybar) for x, yy in zip(xs, ys))

    C = sxy / sxx
    L = ybar - C * xbar

    return L


def tikz_coordinates(ns, values):
    return " ".join(
        f"({int(n)},{v:.16g})"
        for n, v in zip(ns, values)
    )


def make_axis(field, variant, ns, y, L_hat):
    coords = tikz_coordinates(ns, y)

    nmin = int(min(ns))
    nmax = int(max(ns))

    title = rf"{FIELD_LABELS[field]}, {VARIANT_LABELS[variant]}"

    return rf"""
\nextgroupplot[
    title={{{title}}},
]
\addplot+[
    only marks,
    mark=*,
    mark size=0.45pt,
]
coordinates {{{coords}}};

\addplot+[
    no marks,
    dashed,
    thick,
]
coordinates {{({nmin},{L_hat:.16g}) ({nmax},{L_hat:.16g})}};

\node[
    anchor=north east,
    fill=white,
    fill opacity=0.75,
    text opacity=1,
    inner sep=1pt
]
at (rel axis cs:0.95,0.95)
{{$\widehat{{L}}={L_hat:.4g}$}};
"""


def main():
    parser = argparse.ArgumentParser(
        description="Generate compact 3x3 PGFPlots grid for quotient convergence."
    )

    parser.add_argument(
        "--get-d-dir",
        default=".",
        help="Directory containing get_D files",
    )


    parser.add_argument(
        "--csv",
        default="XL-test.csv",
        help="Combined CSV file with columns q,n,m,D,variant,pred_bit_ops",
    )

    parser.add_argument(
        "--output",
        default="quotient_convergence_grid.tex",
        help="Output TikZ/PGFPlots file",
    )

    parser.add_argument(
        "--start",
        type=int,
        default=10,
        help="First parameter value n represented by the first line of each get_D file",
    )

    parser.add_argument(
        "--tail-fraction",
        type=float,
        default=0.4,
        help="Tail fraction used for estimating the limit",
    )

    args = parser.parse_args()

    data_dir = Path(args.get_d_dir)
    csv_file = Path(args.csv)

    prediction_costs = read_prediction_costs_csv(csv_file)
    axes = []

    for field in FIELDS:
        den_file = data_dir / DENOMINATORS[field]
        if not den_file.exists():
            raise FileNotFoundError(den_file)

        den_costs = read_big_o_costs(den_file, args.start)

        for variant in VARIANTS:
            num_costs = prediction_costs[(field, variant)]
            if not num_costs:
                raise CsvFormatError(
                    f"No CSV rows found for q={field}, variant={variant} in {csv_file}"
                )

            samples = quotients_from_costs(num_costs, den_costs)
            ns = [float(n) for n, _ in samples]
            y = [value for _, value in samples]

            L_hat = estimate_limit_n2(
                ns,
                y,
                tail_fraction=args.tail_fraction,
            )

            print(
                f"GF({field:>3s}) {variant:12s} "
                f"L_hat={L_hat:.12g}"
            )

            axes.append(
                make_axis(
                    field=field,
                    variant=variant,
                    ns=ns,
                    y=y,
                    L_hat=L_hat,
                )
            )

    tex = rf"""
% Generated automatically.
% Requires:
% \usepackage{{pgfplots}}
% \pgfplotsset{{compat=1.18}}
% \usepgfplotslibrary{{groupplots}}

\begin{{tikzpicture}}
\begin{{groupplot}}[
    group style={{
        group size=3 by 3,
        horizontal sep=1.5cm,
        vertical sep=1.5cm,
        x descriptions at=edge bottom,
    }},
    width=0.32\textwidth,
    height=0.13\textheight,
    xmode=log,
    grid=both,
    minor grid style={{gray!12}},
    major grid style={{gray!25}},
    tick align=outside,
    tick pos=left,
    xlabel={{Parameter size $n$}},
    ylabel={{Quotient $x_n$}},
    title style={{font=\small}},
    label style={{font=\small}},
    tick label style={{font=\scriptsize}},
]

{''.join(axes)}

\end{{groupplot}}
\end{{tikzpicture}}
"""

    output_file = Path(args.output)
    output_file.write_text(tex)
    print(f"Wrote {output_file}")


if __name__ == "__main__":
    main()

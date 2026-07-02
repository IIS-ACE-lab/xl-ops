#!/usr/bin/env python3

from pathlib import Path
import argparse
import csv
import math


FIELDS = ["2", "31", "256"]
VARIANTS = ["baseline", "const", "const_bucket"]

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

SECURITY_LEVELS = [128, 192, 256]


class CsvFormatError(ValueError):
    pass


def field_label(field):
    if field == "256":
        return r"$\mathrm{GF}(256)$"
    return rf"$\mathrm{{GF}}({field})$"


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
                    f"Expected second whitespace-separated column in {filename}, row {row}"
                )
            costs.append((start_n + len(costs), int(parts[1])))
    return costs


def find_threshold(costs, level):
    threshold = 1 << level

    for n, cost in costs:
        if cost >= threshold:
            return n, cost

    return None, None


def format_result(n, cost, show_cost):
    if n is None:
        return "--"

    if show_cost:
        return rf"{n} ($2^{{{math.log2(cost):.1f}}}$)"

    return str(n)


def latex_header():
    print(r"\begin{tabular}{llccc}")
    print(r"\toprule")
    print(r"Field & Model & $n_{128}$ & $n_{192}$ & $n_{256}$ \\")
    print(r"\midrule")


def latex_footer():
    print(r"\bottomrule")
    print(r"\end{tabular}")


def format_results(costs, show_cost):
    results = []
    for level in SECURITY_LEVELS:
        n, cost = find_threshold(costs, level)
        results.append(format_result(n, cost, show_cost))
    return results


def print_result(field, model, results, latex):
    if latex:
        field_text = field_label(field) if model == "asympt. est." else ""
        print(rf"{field_text} & {model} & {results[0]} & {results[1]} & {results[2]} \\")
    else:
        print(
            f"GF({field:>3}) {model:12s} "
            f"128: {results[0]:>14s}  "
            f"192: {results[1]:>14s}  "
            f"256: {results[2]:>14s}"
        )


def main():
    parser = argparse.ArgumentParser(
        description="Look up n values reaching 128/192/256-bit XL cost levels."
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
        "--start",
        type=int,
        default=10,
        help="First n value represented by the first line of each get_D file",
    )

    parser.add_argument(
        "--latex",
        action="store_true",
        help="Print LaTeX table",
    )

    parser.add_argument(
        "--show-cost",
        action="store_true",
        help="Also show log2(cost) next to each threshold n",
    )

    args = parser.parse_args()
    data_dir = Path(args.get_d_dir)

    csv_file = Path(args.csv)

    prediction_costs = read_prediction_costs_csv(csv_file)

    if args.latex:
        latex_header()

    for field in FIELDS:
        # First line per field: asymptotic big-O model.
        big_o_file = data_dir / DENOMINATORS[field]
        big_o_costs = read_big_o_costs(big_o_file, args.start)
        big_o_results = format_results(big_o_costs, args.show_cost)
        print_result(field, "asympt. est." if args.latex else "big-O", big_o_results, args.latex)

        # Then concrete variants from the combined CSV.
        for variant in VARIANTS:
            pred_costs = prediction_costs[(field, variant)]
            if not pred_costs:
                raise CsvFormatError(
                    f"No CSV rows found for q={field}, variant={variant} in {csv_file}"
                )

            results = format_results(pred_costs, args.show_cost)
            print_result(field, VARIANT_LABELS[variant], results, args.latex)

        if args.latex and field != FIELDS[-1]:
            print(r"\midrule")
        elif not args.latex:
            print()

    if args.latex:
        latex_footer()


if __name__ == "__main__":
    main()

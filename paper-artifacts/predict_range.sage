#!/usr/bin/env sage

import argparse
import csv
import subprocess
import sys
import re


def run_xl_test_f_cost_with_params(exe, q_value, n_value, m_value):
    """
    Run XL_test only in --f-cost mode for the selected q,n,m.

    This avoids full XL_test runs but still lets XL_test determine D and
    print the primitive field-operation bit costs.
    """
    cmd = [
        exe,
        "-q", str(q_value),
        "-n", str(n_value),
        "-m", str(m_value),
        "--f-cost",
    ]

    p = subprocess.run(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )

    if p.returncode != 0:
        raise SystemExit(
            "XL_test --f-cost failed.\n"
            f"command: {' '.join(cmd)}\n"
            f"return code: {p.returncode}\n"
            f"stdout:\n{p.stdout}\n"
            f"stderr:\n{p.stderr}"
        )

    return p.stdout


def parse_f_cost_output_local(text):
    """
    Local parser for XL_test --f-cost output.

    Returns:
        {
          "q": ...,
          "n": ...,
          "m": ...,
          "D": ...,
          "bit_costs": {...}
        }
    """
    def find_number(pattern, name):
        m = re.search(pattern, text, flags=re.MULTILINE)
        if not m:
            raise ValueError(f"Could not parse {name} from --f-cost output.")
        s = m.group(1)
        return float(s) if "." in s else int(s)

    qnmD = re.search(
        r"^q:\s*(\d+)\s+n:\s*(\d+)\s+m:\s*(\d+)\s+D:\s*(\d+)\s*$",
        text,
        flags=re.MULTILINE,
    )
    if not qnmD:
        raise ValueError("Could not parse q,n,m,D header from --f-cost output.")

    return {
        "q": int(qnmD.group(1)),
        "n": int(qnmD.group(2)),
        "m": int(qnmD.group(3)),
        "D": int(qnmD.group(4)),
        "bit_costs": {
            "add": find_number(r"^bit ops add:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops add"),
            "dbl": find_number(r"^bit ops dbl:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops dbl"),
            "sub": find_number(r"^bit ops sub:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops sub"),
            "mul": find_number(r"^bit ops mul:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops mul"),
            "inv": find_number(r"^bit ops inv:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops inv"),
            "mul_const": find_number(
                r"^(?:avg|exact)?\s*bit ops mul const:\s*([0-9]+(?:\.[0-9]+)?)\s*$",
                "bit ops mul const",
            ),
        },
    }


def make_prediction_only_run_for_range(q_value, n_value, m_value, D_value, bit_costs):
    """
    Construct a minimal XLRun object for prediction only.

    Requires XLRun from XL_cost.sage to be in scope.
    """
    return XLRun(
        q=q_value,
        n=n_value,
        m=m_value,
        D=D_value,
        seed=0,
        ncols=0,
        nrows=0,
        bit_costs=dict(bit_costs),
        gf_counts={
            "field add": 0,
            "field dbl": 0,
            "field sub": 0,
            "field mul": 0,
            "field inv": 0,
        },
        field_mixed_mul=0,
        measured_bit_ops=0,
        measured_bm_bit_ops=0,
        measured_la_bit_ops=0,
    )


def variant_flags(variant):
    if variant == "base":
        return False, False
    if variant == "const":
        return True, False
    if variant == "bucket":
        return True, True
    raise ValueError(f"unknown variant: {variant}")


def predict_total_bits_for_variant(run, variant):
    const, bucket = variant_flags(variant)

    pred = select_prediction(run, const=const, bucket=bucket)
    pred = attach_bit_cost_prediction(pred, run, const=const, bucket=bucket)

    return pred.bit_ops_total


def main():
    ap = argparse.ArgumentParser(
        description="Generate predicted total bit-operation counts for a parameter range."
    )

    ap.add_argument(
        "--XL_cost",
        default="../XL_cost.sage",
        help="Path to XL_cost.sage",
    )

    ap.add_argument(
        "--exe",
        default="../src/bin/XL_test",
        help="Path to XL_test executable",
    )

    ap.add_argument(
        "-q", "--field",
        type=int,
        choices=[2, 31, 256],
        default=None,
        help="Field q. If omitted, run all fields: 2, 31, 256.",
    )

    ap.add_argument(
        "--n-min",
        type=int,
        default=10,
        help="First n value",
    )

    ap.add_argument(
        "--n-max",
        type=int,
        default=400,
        help="Last n value, inclusive",
    )

    ap.add_argument(
        "--n-step",
        type=int,
        default=1,
        help="Step size for n",
    )

    ap.add_argument(
        "--m-factor",
        type=int,
        default=2,
        help="Use m = m_factor * n",
    )

    ap.add_argument(
        "--variants",
        default="base,const,bucket",
        help="Comma-separated variants: base,const,bucket",
    )

    ap.add_argument(
        "--output",
        default="-",
        help="CSV output file, or '-' for stdout",
    )

    args = ap.parse_args()

    fields = [args.field] if args.field is not None else [2, 31, 256]

    # Load XL_cost.sage once. This is what avoids repeated Sage startup.
    globals()["XL_COST_LIBRARY_MODE"] = True
    try:
        globals()["XL_COST_FILE"] = "../XL_cost_formulas.sage"
        load(args.XL_cost)
    finally:
        globals()["XL_COST_LIBRARY_MODE"] = False

    variants = [v.strip() for v in args.variants.split(",") if v.strip()]
    for v in variants:
        if v not in ("base", "const", "bucket"):
            raise SystemExit(f"Unknown variant in --variants: {v}")

    out_fh = sys.stdout if args.output == "-" else open(args.output, "w", newline="")
    try:
        writer = csv.writer(out_fh)
        writer.writerow(["q", "n", "m", "D", "variant", "pred_bit_ops"])

        for q_value in fields:
            for n_value in range(args.n_min, args.n_max + 1, args.n_step):
                m_value = args.m_factor * n_value
        
                raw_f_cost = run_xl_test_f_cost_with_params(
                    exe=args.exe,
                    q_value=q_value,
                    n_value=n_value,
                    m_value=m_value,
                )
                fcost = parse_f_cost_output_local(raw_f_cost)
        
                if fcost["q"] != q_value:
                    raise SystemExit(
                        f"--f-cost returned q={fcost['q']}, "
                        f"but requested q={q_value}."
                    )
        
                if fcost["n"] != n_value or fcost["m"] != m_value:
                    raise SystemExit(
                        "XL_test --f-cost returned unexpected n,m.\n"
                        f"requested q={q_value}, n={n_value}, m={m_value}\n"
                        f"returned  q={fcost['q']}, n={fcost['n']}, m={fcost['m']}"
                    )
        
                run = make_prediction_only_run_for_range(
                    q_value=fcost["q"],
                    n_value=fcost["n"],
                    m_value=fcost["m"],
                    D_value=fcost["D"],
                    bit_costs=fcost["bit_costs"],
                )
        
                for variant in variants:
                    pred_bits = predict_total_bits_for_variant(run, variant)
                    writer.writerow([
                        fcost["q"],
                        fcost["n"],
                        fcost["m"],
                        fcost["D"],
                        variant,
                        int(round(pred_bits)),
                    ])


    finally:
        if out_fh is not sys.stdout:
            out_fh.close()


if __name__ == "__main__":
    main()


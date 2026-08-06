#!/usr/bin/env python3

import argparse
import math
import re
import subprocess

LEFT_TYPES = [
    (r"\textbf{Type I}  \\$\mathrm{GF}(2)$\\$m=2n$"   ,   {"q": 2,   "nrange": [80,83,99,100], "m_rule": "2n"}),
    (r"\textbf{Type II} \\$\mathrm{GF}(256)$\\$m=2n$" ,  {"q": 256, "nrange": [36,37,53,54],  "m_rule": "2n"}),
    (r"\textbf{Type III}\\$\mathrm{GF}(31)$\\$m=2n$"  , {"q": 31,  "nrange": [37,38,52,53],  "m_rule": "2n"}),
]

RIGHT_TYPES = [
    (r"\textbf{Type IV}\\$\mathrm{GF}(2)$\\$n\approx1.5m$"  , {"q": 2,   "nrange": [75,76,99,100], "m_rule": "n1p5m"}),
    (r"\textbf{Type V} \\$\mathrm{GF}(256)$\\$n\approx1.5m$",  {"q": 256, "nrange": [19,20,34,35],  "m_rule": "n1p5m"}),
    (r"\textbf{Type VI}\\$\mathrm{GF}(31)$\\$n\approx1.5m$" , {"q": 31,  "nrange": [23,24,35,36],  "m_rule": "n1p5m"}),
]


def run_cmd(cmd):
    p = subprocess.run(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=True,
    )
    return p.stdout.strip()


def m_from_rule(n, rule):
    if rule == "2n":
        return 2 * n

    if rule == "n1p5m":
        # n approx 1.5 m => m approx 2n/3
        return round(2 * n / 3)

    raise ValueError(f"unknown rule: {rule}")


def effective_instance(q, n, m):
    """
    Signature case:
    guess variables until n' = m - 2,
    costing q^2 repetitions.
    """
    if m < n:
        return m - 2, m, q**2

    return n, m, 1


def compute_big_o(get_d, q, n, m):
    out = run_cmd([
        "sage",
        get_d,
        "-q", str(q),
        "-n", str(n),
        "-m", str(m),
        "-s",
    ])

    line = out.strip().splitlines()[-1]
    parts = line.split()

    if len(parts) < 2:
        raise ValueError(
            f"Expected second column from get_D.sage:\n{out}"
        )

    return int(parts[1])


def get_pred_number(text: str) -> int:
    match = re.search(r"^bit ops:\s*\d+\s*\(pred\s+(\d+)\)", text, re.MULTILINE)
    if not match:
        raise ValueError("No matching 'bit ops' line found" + text)
    return int(match.group(1))

def compute_baseline(xl_test, q, n, m):
    out = run_cmd(
        xl_test.split(" ") + [
        "-q", str(q),
        "-n", str(n),
        "-m", str(m),
#        "--GF2-opt" if q==2 else "",
        "--pred",
    ])

    num = get_pred_number(out)

    if not num:
        raise ValueError(
            f"Could not parse prediction output:\n{out}"
        )

    return num

def compute_const_bucket(xl_test, q, n, m):
    out = run_cmd(
        xl_test.split(" ") + [
        "-q", str(q),
        "-n", str(n),
        "-m", str(m),
        "-c",
        "-b",
#        "--GF2-opt" if q==2 else "",
        "--pred",
    ])

    num = get_pred_number(out)

    if not num:
        raise ValueError(
            f"Could not parse prediction output:\n{out}"
        )

    return num

#    nums = [int(tok) for tok in out.split() if tok.isdigit()]
#
#    if not nums:
#        raise ValueError(
#            f"Could not parse prediction output:\n{out}"
#        )
#
#    return nums[-1]


def lg(x):
    return math.log2(x)


def collect_rows(type_name, cfg, top, get_d, xl_test):
    q = cfg["q"]
    nrange = cfg["nrange"]

    rows = []

    for n in nrange:
        m = m_from_rule(n, cfg["m_rule"])

        n_eff, m_eff, repetitions = effective_instance(
            q,
            n,
            m,
        )

#        big_o = repetitions * compute_big_o(
#            get_d,
#            q,
#            n_eff,
#            m_eff,
#        )
        
        big_o = repetitions * compute_baseline(
            xl_test,
            q,
            n_eff,
            m_eff,
        )

        pred = repetitions * compute_const_bucket(
            xl_test,
            q,
            n_eff,
            m_eff,
        )

        rows.append((
            type_name,
            n,
            lg(big_o),
            lg(pred),
        ))

    return rows


def print_header():
    print(r"\begin{tabular}{@{}lrrr@{\hspace*{1cm}}lrrr@{}}")
    print(r"\toprule")
    print(
        r"Type & $n$ & $\log_2(base)$ & $\log_2(\text{c+b})$ & "
        r"Type & $n$ & $\log_2(base)$ & $\log_2(\text{c+b})$ \\"
    )
    print(r"\toprule")


def print_footer():
    print(r"\bottomrule")
    print(r"\end{tabular}")


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--get-d",
        default="get_D.sage",
    )

    parser.add_argument(
        "--xl-test",
        default="../src/bin/XL_test",
    )

    parser.add_argument(
        "--top",
        type=int,
        default=4,
    )

    args = parser.parse_args()

    print_header()

    pairs = list(zip(LEFT_TYPES, RIGHT_TYPES))

    for idx, ((lt, lcfg), (rt, rcfg)) in enumerate(pairs):
        left_rows = collect_rows(
            lt,
            lcfg,
            args.top,
            args.get_d,
            args.xl_test,
        )

        right_rows = collect_rows(
            rt,
            rcfg,
            args.top,
            args.get_d,
            args.xl_test,
        )

        for i in range(args.top):
            ltype, ln, lo, lp = left_rows[i]
            rtype, rn, ro, rp = right_rows[i]

            ltype_col = rf"\multirow{{{args.top}}}{{*}}{{\shortstack{{{ltype}}}}}" if i == 0 else ""
            rtype_col = rf"\multirow{{{args.top}}}{{*}}{{\shortstack{{{rtype}}}}}" if i == 0 else ""

            print(
                rf"{ltype_col} & "
                rf"${ln}$ & "
                rf"${lo:.2f}$ & "
                rf"${lp:.2f}$ & "
                rf"{rtype_col} & "
                rf"${rn}$ & "
                rf"${ro:.2f}$ & "
                rf"${rp:.2f}$ \\"
            )

        if idx + 1 != len(pairs):
            print(r"\midrule")

    print_footer()


if __name__ == "__main__":
    main()

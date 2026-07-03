#!/usr/bin/env sage
# xl_formula_tables.sage
#
# Print LaTeX formula snippets/tables from xl_cost_formulas.sage.
#
# The intent is that all mathematical formulas are sourced from
# xl_cost_formulas.sage; this script only controls presentation.

import argparse
import os
import re
import subprocess


# --------------------------------------------------------------------
# Loading formulas
# --------------------------------------------------------------------

def load_formula_file(path):
    if not os.path.exists(path):
        raise FileNotFoundError(f"Formula file not found: {path}")

    # Sage's load() injects symbols/functions into this session.
    load(path)


# --------------------------------------------------------------------
# LaTeX presentation helpers
# --------------------------------------------------------------------

def latex_single_fraction(expr, transform=factor):
    """
    Render expr as one LaTeX fraction when it has a nontrivial denominator.

    Example:
        3*R_q*(R_q + 1)/2
    becomes something like:
        \\frac{3 R_q (R_q + 1)}{2}

    This is presentation-only; it does not change the source formula.
    """
    e = transform(SR(expr))

    num = e.numerator()
    den = e.denominator()

    if den == 1:
        return latex(num)

    return r"\displaystyle \frac{%s}{%s}" % (latex(num), latex(den))


def make_tex_function(style):
    """
    Return a function expr -> LaTeX string according to the requested style.
    """
    if style == "sage":
        return lambda expr: latex(SR(expr))

    if style == "factor":
        return lambda expr: latex(factor(SR(expr)))

    if style == "expand":
        return lambda expr: latex(expand(SR(expr)))

    if style == "single-fraction":
        return lambda expr: latex_single_fraction(expr)

    raise ValueError(f"Unknown style: {style}")


def tex_zero_aware(tex, expr):
    """
    Keep integer zero as a clean 0 even if expr is a plain Python int.
    """
    if expr == 0:
        return "0"
    return tex(expr)


def tex_with_ell(tex, expr):
    out = tex(expr)
    out = out.replace(r"\mathit{ell}_{2}", r"\ell_2")
    out = out.replace(r"\mathit{ell}_2", r"\ell_2")
    out = out.replace(r"\mathit{ell}", r"\ell")
    return out


# --------------------------------------------------------------------
# Formula access helpers
# --------------------------------------------------------------------

def variant_suffix_latex(variant):
    if variant == "base":
        return r"\mathrm{base}"
    if variant == "const":
        return r"\mathrm{const}"
    if variant == "buck":
        return r"\mathrm{buck}"
    raise ValueError(f"unknown variant: {variant}")


def variant_formulas(variant):
    """
    Return the verified generic formula dictionary for one variant.

    These functions are expected to come from xl_cost_formulas.sage.
    """
    if variant == "base":
        return generic_baseline_formulas()

    if variant == "const":
        return generic_const_formulas()

    if variant == "buck":
        return generic_const_bucket_formulas()

    raise ValueError(f"unknown variant: {variant}")


# --------------------------------------------------------------------
# Table I: BM, normalization, evaluation
# --------------------------------------------------------------------

def print_table_I(tex):
    """
    Print LaTeX table for BM, normalization, and evaluation.

    Uses formulas from xl_cost_formulas.sage:
        A_BM, S_BM, M_BM
        norm_M, norm_I
        eval_A, eval_M
    """

    rows = [
        (r"Berlekamp--Massey", A_BM, S_BM, M_BM, 0),
        (r"normalization", 0, 0, norm_M, norm_I),
        (r"evaluation", eval_A, 0, eval_M, 0),
    ]

    print(r"\[")
    print(r"\begin{array}{l|cccc}")
    print(r" & A & S & M & I \\")
    print(r"\hline")


    for name, A, S, M, I in rows:
        print(
            rf"\text{{{name}}} "
            rf"& {tex_zero_aware(tex, A)} "
            rf"& {tex_zero_aware(tex, S)} "
            rf"& {tex_zero_aware(tex, M)} "
            rf"& {tex_zero_aware(tex, I)} \\"
        )

    print(r"\end{array}")
    print(r"\]")


# --------------------------------------------------------------------
# Table II: LA costs by variant
# --------------------------------------------------------------------

def print_table_II(tex):
    """
    Print LaTeX table for LA operation counts for each variant.

    This derives the displayed LA components from the verified formula
    dictionaries in xl_cost_formulas.sage, rather than duplicating formulas.
    """

    variants = [
        ("baseline", "base"),
        ("const", "const"),
        ("const+bucket", "buck"),
    ]

    print(r"\[")
    print(r"\begin{array}{l|cccc}")
    print(r" & A & M & M^{\mathrm{fix}} \\")
    print(r"\hline")


    for row_name, variant in variants:
        formulas = variant_formulas(variant)

        print(
            rf"\text{{{row_name}}} "
            rf"& {tex_zero_aware(tex, formulas['A_W'])} "
            rf"& {tex_zero_aware(tex, formulas['M_W'])} "
            rf"& {tex_zero_aware(tex, formulas['M_fixed'])} \\"
        )

    print(r"\end{array}")
    print(r"\]")


# --------------------------------------------------------------------
# Full operation-count formulas by variant
# --------------------------------------------------------------------

def structured_all_formula_expr(variant, name, formulas):
    """
    Return the full operation-count formula for --all-* outputs directly
    from the verified formula dictionary returned by xl_cost_formulas.sage.

    The dictionary already exposes both the total counts (A, S, M, I,
    M_fixed) and the LA components (A_W, M_W).  For --all-* we use the
    total counts directly.
    """
    if name in formulas:
        return formulas.get(name, 0)

    raise ValueError(f"unknown formula name: {name}")


def print_all_variant(tex, variant):
    """
    Print a LaTeX align* block with full operation-count formulas.

    Variants:
        base, const, buck

    Uses generic_*_formulas() from xl_cost_formulas.sage and reconstructs
    structured sums from the verified component keys.
    """
    formulas = variant_formulas(variant)
    suffix = variant_suffix_latex(variant)

    rows = [
        ("A", "A"),
        ("S", "S"),
        ("M", "M"),
    ]

    M_fixed = structured_all_formula_expr(variant, "M_fixed", formulas)
    if M_fixed != 0:
        rows.append((r"M_{\mathrm{fixed}}", "M_fixed"))

    rows.append(("I", "I"))

    print(r"\begin{align*}")

    for idx, (lhs_name, key) in enumerate(rows):
        expr = structured_all_formula_expr(variant, key, formulas)
        line_end = r"\\" if idx + 1 < len(rows) else ""
        print(rf"{lhs_name}^{{{suffix}}} &= {tex_zero_aware(tex, expr)} {line_end}")

    print(r"\end{align*}")


# --------------------------------------------------------------------
# Extra BM non-field bit costs
# --------------------------------------------------------------------

def print_extra_BM(tex):
    """
    Print LaTeX formulas for the additional non-field BM bit costs.

    The formulas are taken from xl_cost_formulas.sage via
    bm_extra_bits_symbolic(q), not duplicated here.

    Presentation notation:
        R_2, ell_2 for GF(2)
        R, ell for GF(31), GF(256)
    """

    # Presentation-only symbols.
    var("R R_2 ell ell_2")

    def presented_extra_expr(q_value):
        expr = bm_extra_bits_symbolic(q_value)

        if q_value == 2:
            return expr.subs({
                R_q: R_2,
                k: ell_2,
                g: field_bit_width(q_value),
            })

        return expr.subs({
            R_q: R,
            k: ell,
            g: field_bit_width(q_value),
        })

    E2 = presented_extra_expr(2)
    E31 = presented_extra_expr(31)
    E256 = presented_extra_expr(256)

    print(r"\begin{align*}")
    print(rf"E_2 &= {tex_with_ell(tex, E2)}, \\")
    print(rf"E_{{31}} &= {tex_with_ell(tex, E31)}, \quad\text{{and}} \\")
    print(rf"E_{{256}} &= {tex_with_ell(tex, E256)}.")
    print(r"\end{align*}")


# --------------------------------------------------------------------
# Primitive field-operation cost table
# --------------------------------------------------------------------

def run_xl_f_cost(xl_test_path, q_value):
    """
    Run:
        XL-test -q q_value --f-cost

    and return stdout.
    """
    cmd = [
        xl_test_path,
        "-q", str(q_value),
        "--f-cost",
    ]

    proc = subprocess.run(
        cmd,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )

    if proc.returncode != 0:
        raise RuntimeError(
            "XL-test --f-cost failed\n"
            f"command: {' '.join(cmd)}\n"
            f"return code: {proc.returncode}\n"
            f"stdout:\n{proc.stdout}\n"
            f"stderr:\n{proc.stderr}"
        )

    return proc.stdout


def parse_required_number(pattern, text, name):
    m = re.search(pattern, text, re.MULTILINE)
    if not m:
        raise ValueError(f"Could not parse {name} from XL-test output")
    s = m.group(1)
    if "." in s:
        return float(s)
    return int(s)


def parse_f_cost_output(text):
    """
    Parse output of:
        XL-test -q ... --f-cost

    Expected lines:
        bit ops add: ...
        bit ops dbl: ...
        bit ops sub: ...
        bit ops mul: ...
        bit ops inv: ...
        avg bit ops mul const: ...
    """
    return {
        "add": parse_required_number(r"^bit ops add:\s*([0-9]+(?:\.[0-9]+)?)", text, "bit ops add"),
        "dbl": parse_required_number(r"^bit ops dbl:\s*([0-9]+(?:\.[0-9]+)?)", text, "bit ops dbl"),
        "sub": parse_required_number(r"^bit ops sub:\s*([0-9]+(?:\.[0-9]+)?)", text, "bit ops sub"),
        "mul": parse_required_number(r"^bit ops mul:\s*([0-9]+(?:\.[0-9]+)?)", text, "bit ops mul"),
        "inv": parse_required_number(r"^bit ops inv:\s*([0-9]+(?:\.[0-9]+)?)", text, "bit ops inv"),
        "mul_const": parse_required_number(
            r"^(?:avg|exact)?\s*bit ops mul const:\s*([0-9]+(?:\.[0-9]+)?)",
            text,
            "bit ops mul const",
        ),
    }


def fmt_cost_value(x, max_decimals=2):
    """
    Format costs compactly:
        41.0 -> 41
        38.75 -> 38.75
        153.741935... -> 153.74
    """
    xf = float(x)

    if abs(xf - round(xf)) < 1e-12:
        return str(int(round(xf)))

    s = f"{xf:.{max_decimals}f}"
    s = s.rstrip("0").rstrip(".")
    return s


def mul_const_gt1_cost(q_value, c_mul_const):
    """
    Average constant-multiplication bit cost conditioned on constants > 1.

    The bucket combination step only multiplies by constants 2,...,q-1.
    If c_mul_const is the average over all q constants, the conditional
    average is c_mul_const * q/(q-2) for q > 2.
    For GF(2), there are no such bucket multiplications.
    """
    if q_value == 2:
        return None

    return float(c_mul_const) * float(q_value) / float(q_value - 2)


def field_name_latex(q_value):
    return rf"\text{{GF}}({q_value})"


def print_operation_cost_table(xl_test_path):
    """
    Print LaTeX table with primitive bit-operation costs for GF(2), GF(31),
    and GF(256), using XL-test --f-cost as the data source.
    """
    fields = [2, 31, 256]

    rows = []
    for q_value in fields:
        out = run_xl_f_cost(xl_test_path, q_value)
        costs = parse_f_cost_output(out)
        rows.append((q_value, costs))

    print(r"\begin{tabular}{l@{\hspace*{1em}}"
         r"S[table-format=3]@{\hspace*{1em}}"
         r"S[table-format=2]"
         r"S[table-format=2]"
         r"S[table-format=3]"
         r"S[table-format=4]@{\hspace*{1em}}"
         r"S[table-format=3.2]@{\hspace*{1em}}"
         r"S[table-format=3.2]}")
    print(r"\toprule")
    print(r"{Field} & {\(q\)} & {add} & {sub} & {mul} & {inv} "
          r"& {\makecell{mul const\\\(\mu_q\)}} "
          r"& {\makecell{bucket mul const\\\(\mu_q q/(q-2)\)}} \\")
    print(r"\midrule")

    for q_value, c in rows:
        c_gt1 = mul_const_gt1_cost(q_value, c["mul_const"])
        c_gt1_tex = 0 if c_gt1 is None else fmt_cost_value(c_gt1)

        print(
            rf"${field_name_latex(q_value)}$ "
            rf"& {q_value} "
            rf"& {fmt_cost_value(c['add'])} "
            rf"& {fmt_cost_value(c['sub'])} "
            rf"& {fmt_cost_value(c['mul'])} "
            rf"& {fmt_cost_value(c['inv'])} "
            rf"& {fmt_cost_value(c['mul_const'])} "
            rf"& {c_gt1_tex} \\"
        )

    print(r"\bottomrule")
    print(r"\end{tabular}")


# --------------------------------------------------------------------
# Concrete bit-cost formulas by field and variant
# --------------------------------------------------------------------

def variant_title(variant):
    if variant == "base":
        return "baseline"
    if variant == "const":
        return "const"
    if variant == "buck":
        return "const+bucket"
    raise ValueError(f"unknown variant: {variant}")


def variant_model_label(variant):
    if variant == "base":
        return r"\mathrm{base}"
    if variant == "const":
        return r"\mathrm{const}"
    if variant == "buck":
        return r"\mathrm{buck}"
    raise ValueError(f"unknown variant: {variant}")


def field_lhs_symbol(q_value, variant):
    """
    LHS for the concrete bit-cost formulas.

    I use B_2 for GF(2), matching your screenshot, and C_q for the
    other fields.
    """
    suffix = variant_model_label(variant)

    if q_value == 2:
        return rf"B_2^{{{suffix}}}"

    return rf"C_{{{q_value}}}^{{{suffix}}}"


def field_text(q_value):
    if q_value == 2:
        return r"GF(2)"
    return rf"GF({q_value})"


def mul_const_symbol(q_value):
    """
    Short symbolic name for c_{mul-const} in GF(q).

    For GF(2), this cost is zero.
    """
    if q_value == 2:
        return SR(0)

    return var(f"mu_{q_value}")

def concrete_cost_formula_from_components(formulas, costs, q_value=None, symbolic_mul_const=True):
    if symbolic_mul_const:
        if q_value is None:
            raise ValueError("q_value is required when symbolic_mul_const=True")
        c_mul_const = mul_const_symbol(q_value)
    else:
        c_mul_const = SR(costs["mul_const"])

    return (
        SR(costs["add"]) * SR(formulas.get("A", 0))
        + SR(costs["sub"]) * SR(formulas.get("S", 0))
        + SR(costs["mul"]) * SR(formulas.get("M", 0))
        + SR(costs["inv"]) * SR(formulas.get("I", 0))
        + c_mul_const * SR(formulas.get("M_fixed", 0))
    )


def extra_BM_symbol(q_value):
    if q_value == 2:
        return r"E_2"
    return rf"E_{{{q_value}}}"


def compact_bit_cost_expr(expr):
    """
    Presentation simplification for concrete bit-cost formulas.

    We expand so that terms like 41*(2R_q-1)Z_q and
    207*(2R_q-1)Z_q combine.
    """
    return expand(SR(expr))


def tex_with_pretty_names(s):
    """
    Replace Sage's default LaTeX for selected variable names.
    """
    s = s.replace(r"\mathit{ell}_{2}", r"\ell_2")
    s = s.replace(r"\mathit{ell}_2", r"\ell_2")
    s = s.replace(r"\mathit{ell}", r"\ell")

    s = s.replace(r"\mathit{mu}_{2}", r"\mu_2")
    s = s.replace(r"\mathit{mu}_2", r"\mu_2")
    s = s.replace(r"\mathit{mu}_{31}", r"\mu_{31}")
    s = s.replace(r"\mathit{mu}_{256}", r"\mu_{256}")

    return s


def insert_bucket_mu_factor(s, q_value, variant):
    """
    Presentation-only rewrite for const+bucket bit-cost formulas.

    In the symbolic expression, mu_q denotes the ordinary average
    constant-multiplication cost.  For bucket fixed multiplications, the
    bit-cost factor is q/(q-2) * mu_q.

    This is applied after line breaking so it does not interfere with
    the line-breaking heuristic.
    """
    if variant != "buck" or q_value == 2:
        return s

    factor = rf"\frac{{{q_value}}}{{{q_value - 2}}}"
    return s.replace(
        rf"\mu_{{{q_value}}}",
        rf"\mu_{{{q_value}}}{factor}",
    )


def specialize_bit_cost_expr_for_field(expr, q_value):
    """
    Presentation-only specialization for field-specific bit-cost formulas.

    We substitute concrete field-size constants q and g=ceil(log_2(q))
    while keeping R_q and Z_q symbolic.  The latter are renamed to
    R_2/R_31/R_256 and Z_2/Z_31/Z_256 at the LaTeX-string level.
    """
    return SR(expr).subs({
        q: ZZ(q_value),
        g: ZZ(field_bit_width(q_value)),
    })


def insert_field_specific_RZ_names(s, q_value):
    """
    Replace generic R_q, Z_q by field-specific R_2, R_31, R_256, etc.
    """
    q_tex = str(q_value)

    replacements = [
        (r"R_{q}", rf"R_{{{q_tex}}}"),
        (r"Z_{q}", rf"Z_{{{q_tex}}}"),
        (r"R_q", rf"R_{{{q_tex}}}"),
        (r"Z_q", rf"Z_{{{q_tex}}}"),
        (r"R_{\mathit{q}}", rf"R_{{{q_tex}}}"),
        (r"Z_{\mathit{q}}", rf"Z_{{{q_tex}}}"),
        (r"\mathit{R}_{q}", rf"R_{{{q_tex}}}"),
        (r"\mathit{Z}_{q}", rf"Z_{{{q_tex}}}"),
        (r"\mathit{R}_{\mathit{q}}", rf"R_{{{q_tex}}}"),
        (r"\mathit{Z}_{\mathit{q}}", rf"Z_{{{q_tex}}}"),
    ]

    for old, new in replacements:
        s = s.replace(old, new)

    return s


def tex_round_floats(expr, tex, decimals=2):
    """
    Convert a Sage expression to LaTeX and shorten decimal constants.
    Also renders variables named ell and ell_2 as \\ell and \\ell_2.
    """
    import re

    s = tex(expr)
    s = tex_with_pretty_names(s)

    def repl(m):
        x = float(m.group(0))
        if abs(x - round(x)) < 1e-12:
            return str(int(round(x)))

        out = f"{x:.{decimals}f}"
        return out.rstrip("0").rstrip(".")

    return re.sub(r"(?<![A-Za-z])\d+\.\d+(?![A-Za-z])", repl, s)


def split_latex_terms(s):
    """
    Split a LaTeX expression into top-level additive terms.

    This is intentionally simple: it tracks brace/parenthesis depth and only
    splits on + or - at depth zero. The sign stays with the following term.
    """
    terms = []
    cur = []
    depth = 0

    for i, ch in enumerate(s):
        if ch in "{[(":
            depth += 1
        elif ch in "}])":
            depth = max(0, depth - 1)

        if depth == 0 and ch in "+-" and cur:
            term = "".join(cur).strip()
            if term:
                terms.append(term)
            cur = [ch]
        else:
            cur.append(ch)

    term = "".join(cur).strip()
    if term:
        terms.append(term)

    return terms


def latex_broken_rhs(rhs, terms_per_line=6, indent=r"&\quad "):
    """
    Turn an RHS LaTeX string into an aligned multi-line RHS.

    Example output:
        \begin{aligned}[t]
          &term1 + term2 + term3 \\
          &\quad + term4 - term5 + term6
        \end{aligned}
    """
    terms = split_latex_terms(rhs)

    if len(terms) <= terms_per_line:
        return rhs

    lines = []
#    for i in range(0, len(terms), terms_per_line):
#        chunk = terms[i:i + terms_per_line]
#        line = " ".join(chunk)
#
#        if i == 0:
#            lines.append(r"&" + line)
#        else:
#            lines.append(indent + line)

    chunk = terms[0:terms_per_line]
    line = " ".join(chunk)

    lines.append(r"&" + line)

    chunk = terms[terms_per_line:]
    line = " ".join(chunk)

    lines.append(indent + line)

    return (
        r"\begin{aligned}[t]" + "\n"
        + (r" \\" + "\n").join(lines) + "\n"
        + r"\end{aligned}"
    )


def format_bit_cost_rhs(
    rhs_expr,
    tex,
    q_value,
    variant,
    decimals=2,
):
    """
    Format a bit-cost RHS.

    Order matters:
      1. Substitute the concrete field size q.
      2. Convert the Sage expression to LaTeX with atomic mu_q.
      3. Rename R_q/Z_q to field-specific R_2/R_31/R_256 and Z_*. 
      4. Break the additive expression into lines.
      5. Insert the bucket-only q/(q-2) factor next to mu_q.

    This keeps the line-breaking heuristic from seeing the extra fraction,
    but avoids leaving standalone q in field-specific formulas.
    """
    rhs_expr = specialize_bit_cost_expr_for_field(rhs_expr, q_value)

    rhs = tex_round_floats(
        rhs_expr,
        tex,
        decimals=decimals,
    )

    rhs = insert_field_specific_RZ_names(rhs, q_value)

    rhs = latex_broken_rhs(
        rhs,
        BIT_COST_TERMS_PER_LINE.get((q_value, variant), 6),
    )

    rhs = insert_bucket_mu_factor(
        rhs,
        q_value=q_value,
        variant=variant,
    )

    return rhs


# Per-field/per-variant line-breaking policy for --bit-costs.
#
# Keys are:
#   (q_value, variant)
#
# where variant is one of:
#   "base", "const", "buck"
#
# Values are the number of additive terms per line.
BIT_COST_TERMS_PER_LINE = {
    (2,   "base"): 10,
    (2,   "const"): 10,
    (2,   "buck"): 10,

    (31,  "base"): 4,
    (31,  "const"): 5,
    (31,  "buck"): 5,

    (256, "base"): 4,
    (256, "const"): 5,
    (256, "buck"): 4,
}


def print_one_bit_cost_formula(
    tex,
    q_value,
    variant,
    costs,
    inline_extra_BM=False,
    decimals=2,
):
    formulas = variant_formulas(variant)

    expr = concrete_cost_formula_from_components(
        formulas,
        costs,
        q_value=q_value,
    )

    if inline_extra_BM:
        # Presentation-only symbols.
        var("ell ell_2")

        extra = bm_extra_bits_symbolic(q_value)

        if q_value == 2:
            extra = extra.subs({
                k: ell_2,
                g: field_bit_width(q_value),
            })
        else:
            extra = extra.subs({
                k: ell,
                g: field_bit_width(q_value),
            })

        expr = expr + extra
        rhs_expr = compact_bit_cost_expr(expr)
        rhs = format_bit_cost_rhs(
            rhs_expr,
            tex,
            q_value=q_value,
            variant=variant,
            decimals=decimals,
        )
    else:
        rhs_expr = compact_bit_cost_expr(expr)
        rhs = format_bit_cost_rhs(
            rhs_expr,
            tex,
            q_value=q_value,
            variant=variant,
            decimals=decimals,
        )
        rhs = rhs + r" + " + extra_BM_symbol(q_value)

    lhs = field_lhs_symbol(q_value, variant)

    print(rf"{lhs} &= {rhs}")

def print_bit_cost_formulas(
    tex,
    xl_test_path,
    inline_extra_BM=False,
):
    """
    Print concrete bit-cost formulas for GF(2), GF(31), GF(256)
    and all three variants.

    Uses XL-test --f-cost to obtain the primitive operation costs.
    """
    fields = [2, 31, 256]
    variants = ["base", "const", "buck"]

    first = True

    for q_value in fields:
        out = run_xl_f_cost(xl_test_path, q_value)
        costs = parse_f_cost_output(out)

        for variant in variants:
#            if not first:
#                print(r"\\[1ex]")
            first = False

            print(
                rf"\paragraph{{{field_text(q_value)}, {variant_title(variant)} model.}}%"
            )
            print("%")

            print(r"\begin{flalign*}")

            print_one_bit_cost_formula(
                tex=tex,
                q_value=q_value,
                variant=variant,
                costs=costs,
                inline_extra_BM=inline_extra_BM,
            )

            print(r"\end{flalign*}")

# --------------------------------------------------------------------
# Main
# --------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Print LaTeX formula tables/snippets for the XL cost model."
    )

    parser.add_argument(
        "--formula-file",
        default=os.path.join(os.path.dirname(os.path.abspath(__file__)), "xl_cost_formulas.sage"),
        help="Path to xl_cost_formulas.sage",
    )

    parser.add_argument(
        "--table-I",
        action="store_true",
        help="Print the BM/normalization/evaluation operation-count table",
    )

    parser.add_argument(
        "--table-II",
        action="store_true",
        help="Print the LA operation-count table",
    )

    parser.add_argument(
        "--all-base",
        action="store_true",
        help="Print full operation-count formulas for the baseline variant",
    )

    parser.add_argument(
        "--all-const",
        action="store_true",
        help="Print full operation-count formulas for the const variant",
    )

    parser.add_argument(
        "--all-const-bucket",
        action="store_true",
        help="Print full operation-count formulas for the const+bucket variant",
    )

    parser.add_argument(
        "--extra-BM",
        action="store_true",
        help="Print additional non-field BM bit-cost formulas",
    )

    parser.add_argument(
        "--style",
        choices=["sage", "factor", "expand", "single-fraction"],
        default="single-fraction",
        help="LaTeX presentation style for formulas",
    )

    parser.add_argument(
        "--op-cost",
        action="store_true",
        help="Print primitive field-operation bit-cost table using XL-test --f-cost",
    )

    parser.add_argument(
        "--xl-test",
        default="./XL-test",
        help="Path to XL-test executable for --op-cost",
    )

    parser.add_argument(
        "--bit-costs",
        action="store_true",
        help="Print concrete bit-cost formulas for all fields and variants",
    )

    parser.add_argument(
        "--inline-extra-BM",
        action="store_true",
        help="Inline the additional BM non-field cost instead of printing +E_q",
    )

    args = parser.parse_args()

    load_formula_file(args.formula_file)

    tex = make_tex_function(args.style)

    printed_any = False

    if args.table_I:
        print_table_I(tex)
        printed_any = True

    if args.table_II:
        if printed_any:
            print()
        print_table_II(tex)
        printed_any = True

    if args.all_base:
        if printed_any:
            print()
        print_all_variant(tex, "base")
        printed_any = True

    if args.all_const:
        if printed_any:
            print()
        print_all_variant(tex, "const")
        printed_any = True

    if args.all_const_bucket:
        if printed_any:
            print()
        print_all_variant(tex, "buck")
        printed_any = True

    if args.extra_BM:
        if printed_any:
            print()
        print_extra_BM(tex)
        printed_any = True

    if args.op_cost:
        if printed_any:
            print()
        print_operation_cost_table(args.xl_test)
        printed_any = True

    if args.bit_costs:
        if printed_any:
            print()
        print_bit_cost_formulas(
            tex=tex,
            xl_test_path=args.xl_test,
            inline_extra_BM=args.inline_extra_BM,
        )
        printed_any = True

    if not printed_any:
        parser.error(
            "No output selected. Use --table-I, --table-II, "
            "--all-base, --all-const, --all-const-bucket, "
            "--extra-BM, --op-cost, and/or --bit-costs."
        )


if __name__ == "__main__":
    main()

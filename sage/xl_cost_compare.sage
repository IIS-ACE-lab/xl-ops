#!/usr/bin/env sage
# xl_cost_compare.sage

import argparse
import dataclasses
import math
import os
import re
import subprocess
from typing import Dict, Optional

def compare_script_dir():
    if "XL_COST_COMPARE_FILE" in globals():
        return os.path.dirname(os.path.abspath(globals()["XL_COST_COMPARE_FILE"]))

    try:
        return os.path.dirname(os.path.abspath(__file__))
    except NameError:
        return os.getcwd()


FORMULA_FILE = globals().get(
    "XL_COST_FORMULAS_FILE",
    os.path.join(compare_script_dir(), "xl_cost_formulas.sage"),
)

load(FORMULA_FILE)


@dataclasses.dataclass
class XLRun:
#    raw_output: str

    q: int
    n: int
    m: int
    D: int
    seed: int

    ncols: int
    nrows: int

    bit_costs: Dict[str, float]
    gf_counts: Dict[str, int]

    field_mixed_mul: int

    measured_bit_ops: int
    measured_bm_bit_ops: int
    measured_la_bit_ops: int

    macaulay_mul_bit_ops: Optional[int] = None
    macaulay_mul_counts: Optional[Dict[str, int]] = None
    macaulay_not_zero: Optional[int] = None

    # Runtime-data prediction overrides.  These are set only when
    # --use-macaulay-runtime-data is requested.
    nominal_mul_const_cost: Optional[float] = None
#    mul_const_cost_from_data: bool = False
    bucket_A_W_nominal: Optional[float] = None
    bucket_A_W_from_data: Optional[float] = None
    use_macaulay_runtime_data: bool = False


@dataclasses.dataclass
class Prediction:
    mode: str

    A: float
    S: float
    M: float
    I: float
    M_fixed: float = 0.0

    BM_extra_bits: float = 0.0

    generic_formulas: Optional[dict] = None
#    specialized_formulas: Optional[dict] = None

#    total_bit_formula_generic: Optional[object] = None
#    total_bit_formula_specialized: Optional[object] = None
#    total_bit_formula_generic_concrete: Optional[object] = None
#    total_bit_formula_specialized_concrete: Optional[object] = None

    bit_ops_total: float = 0.0
    bit_ops_breakdown: Optional[Dict[str, float]] = None


# ---------------------------------------------------------------------------
# Running XL-test
# ---------------------------------------------------------------------------

def run_xl_test(
    exe: str,
    q: int,
    n: int,
    m: int,
    seed: int = 0,
    perm: Optional[int] = None,
    const: bool = False,
    bucket: bool = False,
    exact_const: bool = False,
    avg_const: bool = False,
    trace: bool = False,
) -> str:
    cmd = [exe, "-q", str(q), "-n", str(n), "-m", str(m), "-s", str(seed)]

    if perm is not None:
        cmd += ["--perm", str(perm)]
    if const:
        cmd.append("-c")
    if bucket:
        cmd.append("-b")
    if exact_const:
        cmd.append("-e")
    elif avg_const:
        cmd.append("-a")
#    if q==2:
#        cmd.append("--GF2-opt")
    if trace:
        cmd.append("--trace")

    proc = subprocess.run(
        cmd,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )

    if proc.returncode != 0:
        raise RuntimeError(
            "XL-test failed\n"
            f"command: {' '.join(cmd)}\n"
            f"return code: {proc.returncode}\n"
            f"stdout:\n{proc.stdout}\n"
            f"stderr:\n{proc.stderr}"
        )

    return proc.stdout

def run_xl_test_f_cost(exe: str, n_value: int, m_value, q_value: int) -> str:
    cmd = [exe, "-q", str(q_value), "-n", str(n_value), "-m", str(m_value), "--f-cost"]

    p = subprocess.run(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )

    if p.returncode != 0:
        raise SystemExit(
            "XL-test --f-cost failed.\n"
            f"command: {' '.join(cmd)}\n"
            f"return code: {p.returncode}\n"
            f"stdout:\n{p.stdout}\n"
            f"stderr:\n{p.stderr}"
        )

    return p.stdout

# ---------------------------------------------------------------------------
# Parsing helpers
# ---------------------------------------------------------------------------

def require_match(pattern: str, text: str, name: str) -> re.Match:
    match = re.search(pattern, text, re.MULTILINE)
    if not match:
        raise ValueError(f"Could not parse {name} using pattern: {pattern}")
    return match


def parse_key_value_block(text: str, header: str) -> Dict[str, int]:
    pattern = rf"^{re.escape(header)}:\s*\n((?:^[^\n:]+:\s*[0-9]+(?:\.[0-9]+)?\s*$\n?)+)"
    match = re.search(pattern, text, re.MULTILINE)
    if not match:
        raise ValueError(f"Could not parse block {header!r}")

    out: Dict[str, int] = {}
    for line in match.group(1).splitlines():
        if not line.strip():
            continue
        key, value = line.split(":", 1)
        out[key.strip()] = int(float(value.strip()))
    return out


def parse_bit_costs(text: str) -> Dict[str, float]:
    costs: Dict[str, float] = {}

    patterns = {
        "add": r"^bit ops add:\s*([0-9]+(?:\.[0-9]+)?)",
        "dbl": r"^bit ops dbl:\s*([0-9]+(?:\.[0-9]+)?)",
        "sub": r"^bit ops sub:\s*([0-9]+(?:\.[0-9]+)?)",
        "mul": r"^bit ops mul:\s*([0-9]+(?:\.[0-9]+)?)",
        "inv": r"^bit ops inv:\s*([0-9]+(?:\.[0-9]+)?)",
    }

    for name, pattern in patterns.items():
        costs[name] = float(require_match(pattern, text, f"bit cost {name}").group(1))

    const_match = re.search(
        r"^(?:avg|exact)?\s*bit ops mul const:\s*([0-9]+(?:\.[0-9]+)?)",
        text,
        re.MULTILINE,
    )
    costs["mul_const"] = float(const_match.group(1)) if const_match else costs["mul"]
    return costs

def parse_optional_int_line(pattern: str, text: str) -> Optional[int]:
    m = re.search(pattern, text, re.MULTILINE)
    if not m:
        return None
    return int(m.group(1))


def parse_macaulay_mul_counts(text: str) -> Optional[Dict[str, int]]:
    """
    Parse optional lines such as:

        Macauly mul field add: 3548
        Macauly mul field dbl: 6886
        ...

    Accepts both 'Macauly' and 'Macaulay'.
    """
    prefix = r"(?:Macauly|Macaulay) mul field"

    counts = {}
    names = {
        "field add": "add",
        "field dbl": "dbl",
        "field sub": "sub",
        "field mul": "mul",
        "field inv": "inv",
    }

    found = False

    for printed_name, short_name in names.items():
        pat = rf"^{prefix} {short_name}:\s*(\d+)"
        m = re.search(pat, text, re.MULTILINE)
        if m:
            counts[printed_name] = int(m.group(1))
            found = True

    return counts if found else None


def parse_output(text: str) -> XLRun:
    header = require_match(
        r"^q:\s*(\d+)\s+n:\s*(\d+)\s+m:\s*(\d+)\s+D:\s*(\d+)",
        text,
        "header",
    )
    seed_match = require_match(r"^seed:\s*(\d+)", text, "seed")
    dims = require_match(
        r"^n\s+(\d+)\s+d\s+(\d+)\s+ncols\s+(\d+)\s+nrows\s+(\d+)",
        text,
        "ncols/nrows",
    )

    field_mixed_match = re.search(r"^field mixed mul:\s*(\d+)", text, re.MULTILINE)

    macaulay_mul_bit_ops = parse_optional_int_line(
        r"^(?:Macauly|Macaulay) mul bit ops:\s*(\d+)",
        text,
    )
    
    macaulay_mul_counts = parse_macaulay_mul_counts(text)

    macaulay_not_zero = parse_optional_int_line(
        r"^(?:Macauly|Macaulay) not zero:\s*(\d+)",
        text,
    )

    return XLRun(
#        raw_output=text,
        q=int(header.group(1)),
        n=int(header.group(2)),
        m=int(header.group(3)),
        D=int(header.group(4)),
        seed=int(seed_match.group(1)),
        ncols=int(dims.group(3)),
        nrows=int(dims.group(4)),
        bit_costs=parse_bit_costs(text),
        gf_counts=parse_key_value_block(text, "GF"),
        field_mixed_mul=int(field_mixed_match.group(1)) if field_mixed_match else 0,
        measured_bit_ops=int(require_match(r"^bit ops:\s*(\d+)", text, "bit ops").group(1)),
        measured_bm_bit_ops=int(require_match(r"^bm bit ops:\s*(\d+)", text, "bm bit ops").group(1)),
        measured_la_bit_ops=int(require_match(r"^LA bit ops:\s*(\d+)", text, "LA bit ops").group(1)),
        macaulay_mul_bit_ops=macaulay_mul_bit_ops,
        macaulay_mul_counts=macaulay_mul_counts,
        macaulay_not_zero=macaulay_not_zero,
    )


def parse_f_cost_output(text: str) -> dict:
    def find_number(pattern: str, name: str):
        m = re.search(pattern, text, flags=re.MULTILINE)
        if not m:
            raise ValueError(f"Could not parse {name} from --f-cost output.")
        s = m.group(1)
        if "." in s:
            return float(s)
        return int(s)

    header = require_match(
        r"^q:\s*(\d+)\s+n:\s*(\d+)\s+m:\s*(\d+)\s+D:\s*(\d+)",
        text,
        "header",
    )

    return {
        "q":   int(header.group(1)),
        "n":   int(header.group(2)),
        "m":   int(header.group(3)),
        "D":   int(header.group(4)),
        "add": find_number(r"^bit ops add:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops add"),
        "dbl": find_number(r"^bit ops dbl:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops dbl"),
        "sub": find_number(r"^bit ops sub:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops sub"),
        "mul": find_number(r"^bit ops mul:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops mul"),
        "inv": find_number(r"^bit ops inv:\s*([0-9]+(?:\.[0-9]+)?)\s*$", "bit ops inv"),
        "mul_const": find_number(
            r"^(?:avg|exact)?\s*bit ops mul const:\s*([0-9]+(?:\.[0-9]+)?)\s*$",
            "bit ops mul const",
        ),
    }

def make_prediction_only_run(args, bit_costs: dict) -> XLRun:
    """
    Construct a minimal XLRun for --pred mode.

    Measured counters are set to zero. The symbolic/numeric prediction
    is still computed normally from q,n,m,D and bit_costs.
    """
    return XLRun(
        q=bit_costs["q"],
        n=bit_costs["n"],
        m=bit_costs["m"],
        D=bit_costs["D"],
        seed=getattr(args, "seed", 0),
        ncols = 0,
        nrows = 0,

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

# ---------------------------------------------------------------------------
# Prediction and cost formulas
# ---------------------------------------------------------------------------

def prediction_mode_name(q_value: int, const: bool, bucket: bool) -> str:
    if q_value == 2:
        if const and bucket:
            return "GF(2) const+bucket"
        if const:
            return "GF(2) const"
        return "GF(2) baseline"

    if const and bucket:
        return "non-binary const+bucket"
    if const:
        return "non-binary const"
    return "non-binary baseline"


def measured_cost_subs_from_bit_costs(bit_costs):
    return {
        C_A: real_or_int(bit_costs["add"]),
        C_S: real_or_int(bit_costs["sub"]),
        C_M: real_or_int(bit_costs["mul"]),
        C_I: real_or_int(bit_costs["inv"]),
        C_M_fixed: real_or_int(bit_costs["mul_const"]),
    }


def concretize_bit_cost_formula(expr, bit_costs):
    return expr.subs(measured_cost_subs_from_bit_costs(bit_costs))


def select_prediction(run: XLRun, const: bool, bucket: bool) -> Prediction:
    generic = select_generic_formulas(const=const, bucket=bucket)
    specialized = select_field_specialized_formulas(run.q, const=const, bucket=bucket)

#    total_generic = total_bit_formula(generic, run.q)
#    total_specialized = total_bit_formula(specialized, run.q)
#    total_generic_concrete = concretize_bit_cost_formula(total_generic, run.bit_costs)
#    total_specialized_concrete = concretize_bit_cost_formula(total_specialized, run.bit_costs)

    R_val, Z_val = model_R_Z_numeric(run.q, run.n, run.m, run.D)
    numeric_subs = {
        R_q: R_val,
        Z_q: Z_val,
        q: ZZ(run.q),
        n: ZZ(run.n),
        m: ZZ(run.m),
        d: ZZ(run.D),
    }

    bm_extra = bm_extra_bits_numeric(run.q, R_val)

    numeric = {}
    for name, expr in generic.items():
        e = expr.subs(numeric_subs) if hasattr(expr, "subs") else expr
        vars_left = remaining_variables(e)
        if len(vars_left) != 0:
            raise ValueError(
                "Generic formula did not evaluate numerically.\n"
                f"name: {name}\n"
                f"expression after substitution: {e}\n"
                f"remaining variables: {vars_left}"
            )
        numeric[name] = RR(e)

    return Prediction(
        mode=prediction_mode_name(run.q, const, bucket),
        A=float(numeric.get("A", 0)),
        S=float(numeric.get("S", 0)),
        M=float(numeric.get("M", 0)),
        I=float(numeric.get("I", 0)),
        M_fixed=float(numeric.get("M_fixed", 0)),
        BM_extra_bits=float(bm_extra),
        generic_formulas=generic,
#        specialized_formulas=specialized,
#        total_bit_formula_generic=total_generic,
#        total_bit_formula_specialized=total_specialized,
#        total_bit_formula_generic_concrete=total_generic_concrete,
#        total_bit_formula_specialized_concrete=total_specialized_concrete,
    )


def attach_bit_cost_prediction(pred: Prediction, run: XLRun) -> Prediction:
    c = run.bit_costs

    add_cost = pred.A * c["add"]
    sub_cost = pred.S * c["sub"]
    mul_cost = pred.M * c["mul"]
    inv_cost = pred.I * c["inv"]
    fixed_mul_cost = pred.M_fixed * c["mul_const"]
    bm_extra_cost = pred.BM_extra_bits

    total = (
        add_cost
        + sub_cost
        + mul_cost
        + inv_cost
        + fixed_mul_cost
        + bm_extra_cost
    )

    pred.bit_ops_breakdown = {
        "A": add_cost,
        "S": sub_cost,
        "M": mul_cost,
        "I": inv_cost,
        "M_fixed": fixed_mul_cost,
        "BM_extra": bm_extra_cost,
    }

    pred.bit_ops_total = total

    return pred


def effective_mul_const_from_macaulay(run: XLRun, const: bool, bucket: bool) -> Optional[float]:
    """
    Recover data-dependent average bit cost of one constant multiplication
    from the optional Macaulay vector-matrix multiplication diagnostic.

    For const baseline:
        Macaulay mul bit ops / Z_q

    For bucket mode this may need a different denominator, depending on
    what exactly XL-test prints for the diagnostic.
    """
    if run.macaulay_mul_bit_ops is None:
        return None

    R_val, Z_val = model_R_Z_numeric(run.q, run.n, run.m, run.D)

    if const and not bucket:
        denom = float(Z_val)
    else:
        # In bucket mode, the Macaulay diagnostic is not a plain
        # "number of fixed multiplications times one cost" quantity.
        # Use 'Macaulay not zero' instead; see
        # bucket_A_W_from_macaulay_not_zero().
        return None

    if denom == 0:
        return None

    return float(run.macaulay_mul_bit_ops) / denom


def select_nominal_A_W_count(run: XLRun, const: bool, bucket: bool) -> float:
    """
    Evaluate the nominal LA addition count A_W from the generic formulas.
    """
    R_val, Z_val = model_R_Z_numeric(run.q, run.n, run.m, run.D)

    subs = {
        R_q: R_val,
        Z_q: Z_val,
        q: ZZ(run.q),
        n: ZZ(run.n),
        m: ZZ(run.m),
        d: ZZ(run.D),
    }

    generic = select_generic_formulas(const=const, bucket=bucket)
    return float(RR(generic["A_W"].subs(subs)))


def bucket_A_W_from_macaulay_not_zero(run: XLRun) -> Optional[float]:
    """
    Runtime-data LA addition count for const+bucket mode.

    The nominal formula uses the expected number of nonzero Macaulay entries
    per Macaulay multiplication:

        ((q - 1)/q) Z_q.

    With runtime data, replace that expected value by the actual diagnostic
    printed by XL-test:

        N_nonzero = Macaulay not zero.

    Thus

        A_W,data = (2 R_q - 1) * (N_nonzero + (q - 2) R_q).

    For GF(2), the bucket-combine term is zero.
    """
    if run.macaulay_not_zero is None:
        return None

    R_val, _ = model_R_Z_numeric(run.q, run.n, run.m, run.D)

    qv = ZZ(run.q)
    Rf = RR(R_val)
    N_nonzero = RR(run.macaulay_not_zero)

    return float(RR(2*R_val - 1) * (N_nonzero + RR(qv - 2) * Rf))


def mul_const_cost_gt1_from_nominal(q_value: int, nominal_mul_const: float) -> Optional[float]:
    """
    Convert the average const-multiplication cost over all field elements
    into the average conditioned on constants > 1.

    This assumes constants 0 and 1 cost zero in the average reported by
    XL-test, which is the convention used in the bucket diagnostic.
    """
    qv = int(q_value)
    if qv <= 2:
        return None
    return float(nominal_mul_const) * qv / (qv - 2)


def apply_runtime_data_for_regular_prediction(
    run: XLRun,
    const: bool,
    bucket: bool,
) -> None:
    """
    Apply Macaulay runtime diagnostics to the regular prediction path.

    Plain const mode:
        replace c_mul_const by

            Macaulay mul bit ops / Z_q.

    Const+bucket mode:
        replace the expected nonzero-entry contribution in A_W by the
        actual 'Macaulay not zero' diagnostic, and replace c_mul_const by
        the average const-multiplication cost conditioned on constants > 1.

    The function mutates run before select_prediction()/attach_bit_cost_prediction()
    are called.  The nominal symbolic formulas remain the source of the
    default prediction; data-dependent changes are applied in a separate
    adjustment step.
    """
    if not const:
        raise SystemExit(
            "--use-macaulay-runtime-data requires --const."
        )

    run.use_macaulay_runtime_data = True
    run.nominal_mul_const_cost = float(run.bit_costs["mul_const"])
    run.bit_costs = dict(run.bit_costs)

    if not bucket:
        eff = effective_mul_const_from_macaulay(
            run,
            const=const,
            bucket=bucket,
        )

        if eff is None:
            raise SystemExit(
                "--use-macaulay-runtime-data requested, but XL-test output did "
                "not contain a usable 'Macaulay mul bit ops' diagnostic."
            )

        run.bit_costs["mul_const"] = float(eff)
#        run.mul_const_cost_from_data = True
        return

    # const+bucket mode.
    A_W_data = bucket_A_W_from_macaulay_not_zero(run)
    if A_W_data is None:
        raise SystemExit(
            "--use-macaulay-runtime-data requested for --const --bucket, but "
            "XL-test output did not contain a usable 'Macaulay not zero' diagnostic."
        )

    run.bucket_A_W_nominal = select_nominal_A_W_count(
        run,
        const=const,
        bucket=bucket,
    )
    run.bucket_A_W_from_data = float(A_W_data)

    if run.q == 2:
        # There are no constants > 1 in GF(2), and M_fixed should be zero.
        run.bit_costs["mul_const"] = 0.0
    else:
        gt1 = mul_const_cost_gt1_from_nominal(
            run.q,
            run.nominal_mul_const_cost,
        )
        if gt1 is None:
            raise SystemExit(
                "Could not derive const-multiplication cost conditioned on constants > 1."
            )
        run.bit_costs["mul_const"] = float(gt1)
#        run.mul_const_cost_from_data = True


def apply_data_dependent_prediction_adjustments(
    pred: Prediction,
    run: XLRun,
    const: bool,
    bucket: bool,
) -> Prediction:
    """
    Apply runtime-data operation-count corrections after nominal prediction
    selection and before bit-cost attachment.

    This keeps select_prediction() purely nominal.
    """
    if const and bucket and run.bucket_A_W_from_data is not None:
        if run.bucket_A_W_nominal is None:
            run.bucket_A_W_nominal = select_nominal_A_W_count(
                run,
                const=const,
                bucket=bucket,
            )

        delta_A_W = float(run.bucket_A_W_from_data) - float(run.bucket_A_W_nominal)
        pred.A += delta_A_W

    return pred

def macaulay_mul_bit_ops_from_counts(run: XLRun) -> Optional[float]:
    if not run.macaulay_mul_counts:
        return None

    c = run.bit_costs
    gf = run.macaulay_mul_counts

    return (
        gf.get("field add", 0) * c["add"]
        + gf.get("field dbl", 0) * c["dbl"]
        + gf.get("field sub", 0) * c["sub"]
        + gf.get("field mul", 0) * c["mul"]
        + gf.get("field inv", 0) * c["inv"]
    )


# ---------------------------------------------------------------------------
# Formatting and LaTeX helpers
# ---------------------------------------------------------------------------

def real_or_int(value):
    value = float(value)
    rounded_int = round(value)
    if abs(value - rounded_int) < 1e-9:
        return ZZ(rounded_int)
    return RR(value)


def rel_error(predicted: float, measured: float) -> float:
    return math.nan if measured == 0 else (predicted - measured) / measured


def ratio(predicted: float, measured: float) -> float:
    return math.nan if measured == 0 else predicted / measured


def fmt_intlike(value: float) -> str:
    if abs(value - round(value)) < 1e-9:
        return str(int(round(value)))
    return f"{value:.3f}"


def shorten_decimal_token(token: str) -> str:
    value = float(token)
    rounded = round(value, 2)
    if abs(rounded - round(rounded)) < 1e-9:
        return str(int(round(rounded)))
    return f"{rounded:.2f}".rstrip("0").rstrip(".")


def shorten_decimal_string(match):
    return shorten_decimal_token(match.group(0))


_DECIMAL_RE = re.compile(r"(?<![A-Za-z_])[-+]?(?:\d+\.\d*|\d*\.\d+)(?:[eE][-+]?\d+)?")


def short_decimal_text(text) -> str:
    return _DECIMAL_RE.sub(shorten_decimal_string, str(text))


def short_latex(expr) -> str:
    return short_decimal_text(latex(expr))


def choose_transform(transform_name: str):
    if transform_name == "factor":
        return factor
    if transform_name == "expand":
        return expand
    if transform_name == "simplify":
        return lambda x: x.simplify_full() if hasattr(x, "simplify_full") else x
    return lambda x: x


def simplify_bit_cost_expr(expr, keep_RZ=True):
    """
    For compact RZ bit-cost formulas, expand after concrete-cost substitution
    so terms such as 92*(R_q+2)*R_q and 61.5*(R_q+1)*R_q combine.
    """
    e = SR(expr)
    if keep_RZ:
        e = e.expand()
    return e


def mode_suffix(const=False, bucket=False):
    if const and bucket:
        return "buck"
    if const:
        return "const"
    return "base"


def total_cost_lhs_latex(q_value, const=False, bucket=False, symbolic_q=False):
    q_part = "q" if symbolic_q else str(q_value)
    return r"C_{%s}^{\mathrm{%s}}" % (q_part, mode_suffix(const=const, bucket=bucket))


def op_formula_lhs_latex(name, suffix):
    lhs_name = "M_{\\mathrm{fixed}}" if name == "M_fixed" else name
    return r"%s^{\mathrm{%s}}" % (lhs_name, suffix)


def op_formulas_latex_block(const=False, bucket=False, transform=None):
    if transform is None:
        transform = lambda x: x

    suffix = mode_suffix(const=const, bucket=bucket)
    formulas = select_generic_formulas(const=const, bucket=bucket)
    order = ["A", "S", "M", "M_fixed", "I"]

    lines = [r"\begin{align*}"]
    first = True
    for name in order:
        expr = formulas.get(name, 0)
        if name == "M_fixed" and expr == 0:
            continue

        lhs = op_formula_lhs_latex(name, suffix)
        rhs = short_latex(transform(expr))

        if first:
            lines.append(r"%s &= %s" % (lhs, rhs))
            first = False
        else:
            lines.append(r"\\")
            lines.append(r"%s &= %s" % (lhs, rhs))

    lines.append(r"\end{align*}")
    return "\n".join(lines)


def total_bit_expr_for_output(
    run: XLRun,
    const=False,
    bucket=False,
    keep_RZ=True,
    concrete_costs=True,
):
    formulas = (
        select_generic_formulas(const=const, bucket=bucket)
        if keep_RZ
        else select_field_specialized_formulas(run.q, const=const, bucket=bucket)
    )
    expr = total_bit_formula(formulas, run.q)
    if concrete_costs:
        expr = concretize_bit_cost_formula(expr, run.bit_costs)
    return expr


def total_bit_formula_latex_block_from_run(
    run: XLRun,
    const=False,
    bucket=False,
    transform=None,
    keep_RZ=True,
    concrete_costs=True,
):
    if transform is None:
        transform = lambda x: x

    total = total_bit_expr_for_output(
        run,
        const=const,
        bucket=bucket,
        keep_RZ=keep_RZ,
        concrete_costs=concrete_costs,
    )

    # Apply the requested transform first, then force expansion for compact RZ
    # concrete-cost formulas so like terms combine.  Rounding is applied after
    # this final simplification through short_latex(...).
    total = transform(total)
    if keep_RZ:
        total = simplify_bit_cost_expr(total, keep_RZ=True)

    lhs = total_cost_lhs_latex(
        run.q,
        const=const,
        bucket=bucket,
        symbolic_q=not concrete_costs,
    )
    rhs = short_latex(total)

    return "\n".join([
        r"\begin{align*}",
        r"%s &= %s" % (lhs, rhs),
        r"\end{align*}",
    ])


## Backward-compatible wrapper with explicit failure: run is needed for measured costs.
#def total_bit_formula_latex_block(qq, const=False, bucket=False, transform=None):
#    raise RuntimeError(
#        "total_bit_formula_latex_block now needs the parsed run so it can "
#        "substitute measured cost factors. Use total_bit_formula_latex_block_from_run(run, ...)."
#    )


def print_operation_formulas_generic(pred: Prediction, transform_name: str = "none") -> None:
    transform = choose_transform(transform_name)
    print()
    print("Operation-count formulas with symbolic R_q and Z_q")
    print("==================================================")
    print(f"mode: {pred.mode}")
    print(f"transform: {transform_name}")

    for name in ["A", "S", "M", "M_fixed", "I"]:
        expr = pred.generic_formulas.get(name, 0) if pred.generic_formulas else 0
        if name == "M_fixed" and expr == 0:
            continue
        e = transform(expr)
        print()
        print(name)
        print("-" * len(name))
        print("Sage:")
        print(short_decimal_text(e))
        print()
        print("LaTeX:")
        print(short_latex(e))


def print_total_bit_formula(
    pred: Prediction,
    run: XLRun,
    const=False,
    bucket=False,
    transform_name: str = "none",
    keep_RZ: bool = True,
    concrete_costs: bool = True,
) -> None:
    transform = choose_transform(transform_name)
    expr = total_bit_expr_for_output(
        run,
        const=const,
        bucket=bucket,
        keep_RZ=keep_RZ,
        concrete_costs=concrete_costs,
    )
    expr = transform(expr)
    if keep_RZ:
        expr = simplify_bit_cost_expr(expr, keep_RZ=True)

    lhs = total_cost_lhs_latex(
        run.q,
        const=const,
        bucket=bucket,
        symbolic_q=not concrete_costs,
    )

    print()
    print("Total bit-operation formula")
    print("===========================")
    print(f"mode: {pred.mode}")
    print(f"field: q={run.q}")
    print(f"transform: {transform_name}")
    print(f"keep_RZ: {keep_RZ}")
    print(f"concrete_costs: {concrete_costs}")

    print()
    print("Sage:")
    print(short_decimal_text(expr))

    print()
    print("LaTeX:")
    print(r"%s &= %s" % (lhs, short_latex(expr)))


## Backward-compatible name.
#def print_total_bit_formula_specialized(pred: Prediction, transform_name: str = "none") -> None:
#    print("print_total_bit_formula_specialized now needs the parsed run. Use print_total_bit_formula(...).")


def write_text_file(path, text):
    directory = os.path.dirname(path)
    if directory:
        os.makedirs(directory, exist_ok=True)
    with open(path, "w") as handle:
        handle.write(text)
        handle.write("\n")


def export_formula_tex_files(
    out_dir,
    run: XLRun,
    transform=None,
    keep_RZ=True,
    concrete_costs=True,
):
    modes = [
        ("base", False, False),
        ("const", True, False),
        ("buck", True, True),
    ]

    rz_tag = "RZ" if keep_RZ else "expanded"
    cost_tag = f"q{run.q}" if concrete_costs else "symbolic-costs"

    for suffix, const, bucket in modes:
        op_tex = op_formulas_latex_block(
            const=const,
            bucket=bucket,
            transform=transform,
        )
        write_text_file(os.path.join(out_dir, f"op-formulas-{suffix}.tex"), op_tex)

        bit_tex = total_bit_formula_latex_block_from_run(
            run,
            const=const,
            bucket=bucket,
            transform=transform,
            keep_RZ=keep_RZ,
            concrete_costs=concrete_costs,
        )
        write_text_file(
            os.path.join(out_dir, f"bit-cost-{suffix}-{cost_tag}-{rz_tag}.tex"),
            bit_tex,
        )



def print_binary_model_debug(run):
    if run.q != 2:
        return

    nn = ZZ(run.n)
    mm = ZZ(run.m)
    dd = ZZ(run.D)

    B_d_n = B_squarefree_numeric(nn, dd)
    B_dm2_n = B_squarefree_numeric(nn, dd - 2)
    B_dm2_nm1 = B_squarefree_numeric(nn - 1, dd - 2)
    B_dm2_nm2 = B_squarefree_numeric(nn - 2, dd - 2)

    R_val, Z_val = model_R_Z_numeric(run.q, run.n, run.m, run.D)

    print()
    print("Binary model debug")
    print("==================")
    print(f"B_d(n)              = B_{dd}({nn}) = {B_d_n}")
    print(f"B_(d-2)(n)          = B_{dd-2}({nn}) = {B_dm2_n}")
    print(f"B_(d-2)(n-1)        = B_{dd-2}({nn-1}) = {B_dm2_nm1}")
    print(f"B_(d-2)(n-2)        = B_{dd-2}({nn-2}) = {B_dm2_nm2}")
    print(f"formula R_2(n,d)    = {R_val}")
    print(f"formula Z_2(n,m,d)  = {Z_val}")
    print(f"program nrows       = {run.nrows}")
    print(f"program ncols       = {run.ncols}")


def predicted_bm_counts_and_bits(run: XLRun):
    """
    Predict BM-only operation counts and bit-cost.

    Uses:
        A_BM additions
        S_BM subtractions
        M_BM multiplications
    """
    R_val, Z_val = model_R_Z_numeric(run.q, run.n, run.m, run.D)

    subs = {
        R_q: R_val,
        Z_q: Z_val,
        q: ZZ(run.q),
        n: ZZ(run.n),
        m: ZZ(run.m),
        d: ZZ(run.D),
    }

    A_val = RR(A_BM.subs(subs))
    S_val = RR(S_BM.subs(subs))
    M_val = RR(M_BM.subs(subs))

    field_bit_ops = (
        A_val * run.bit_costs["add"]
        + S_val * run.bit_costs["sub"]
        + M_val * run.bit_costs["mul"]
    )

    extra_bits = RR(bm_extra_bits_numeric(run.q, R_val))

    counts = {
        "A_BM": A_val,
        "S_BM": S_val,
        "M_BM": M_val,
        "BM_extra": extra_bits,
    }

    return counts, RR(field_bit_ops + extra_bits)

def predicted_la_counts_and_bits(run: XLRun, const: bool, bucket: bool):
    """
    Predict LA-only operation counts and bit-cost.

    Baseline:
        A_W additions
        A_M ordinary multiplications

    Const / bucket:
        A_W additions
        M_W_fixed constant multiplications
    """
    R_val, Z_val = model_R_Z_numeric(run.q, run.n, run.m, run.D)

    subs = {
        R_q: R_val,
        Z_q: Z_val,
        q: ZZ(run.q),
        n: ZZ(run.n),
        m: ZZ(run.m),
        d: ZZ(run.D),
    }

    if const and bucket:
        f = generic_const_bucket_formulas()
    elif const:
        f = generic_const_formulas()
    else:
        f = generic_baseline_formulas()

    A_W_expr = f["A_W"]
    A_M_expr = f["M_W"]
    M_W_fixed_expr = f['M_fixed']

    A_W_val = RR(A_W_expr.subs(subs))
    A_M_val = RR(A_M_expr.subs(subs))
    M_W_fixed_val = RR(M_W_fixed_expr.subs(subs))

    if const and bucket and run.bucket_A_W_from_data is not None:
        A_W_val = RR(run.bucket_A_W_from_data)

    bit_ops = (
        A_W_val * run.bit_costs["add"]
        + A_M_val * run.bit_costs["mul"]
        + M_W_fixed_val * run.bit_costs["mul_const"]
    )

    counts = {
        "A_W": A_W_val,
        "A_M": A_M_val,
        "M_W_fixed": M_W_fixed_val,
    }

    return counts, RR(bit_ops)

# ---------------------------------------------------------------------------
# Reporting
# ---------------------------------------------------------------------------

# def bit_cost_prediction_with_mul_const(pred: Prediction, run: XLRun, mul_const_cost: float):
#     """
#     Diagnostic-only recomputation of predicted bit ops using a replacement
#     constant-multiplication cost.
# 
#     Does not modify pred or run.
#     """
#     c = run.bit_costs
# 
#     add_cost = pred.A * c["add"]
#     sub_cost = pred.S * c["sub"]
#     mul_cost = pred.M * c["mul"]
#     inv_cost = pred.I * c["inv"]
#     fixed_mul_cost = pred.M_fixed * mul_const_cost
#     bm_extra_cost = getattr(pred, "BM_extra_bits", 0.0)
# 
#     total = (
#         add_cost
#         + sub_cost
#         + mul_cost
#         + inv_cost
#         + fixed_mul_cost
#         + bm_extra_cost
#     )
# 
#     return {
#         "A": add_cost,
#         "S": sub_cost,
#         "M": mul_cost,
#         "I": inv_cost,
#         "M_fixed": fixed_mul_cost,
#         "BM_extra": bm_extra_cost,
#         "total": total,
#     }
# 
# def predicted_bm_bits_with_mul_const(run: XLRun, mul_const_cost: float):
#     """
#     BM does not use M_fixed, so this is normally identical to the usual BM
#     prediction. Kept for diagnostic table symmetry.
#     """
#     bm_counts, bm_bits = predicted_bm_counts_and_bits(run)
#     return bm_bits
# 
# def predicted_la_bits_with_mul_const(
#     run: XLRun,
#     const: bool,
#     bucket: bool,
#     mul_const_cost: float,
# ):
#     """
#     Diagnostic-only LA bit prediction using a replacement const-mul cost.
#     """
#     la_counts, _ = predicted_la_counts_and_bits(
#         run,
#         const=const,
#         bucket=bucket,
#     )
# 
#     A_W = float(la_counts["A_W"])
#     A_M = float(la_counts["A_M"])
#     M_W_fixed = float(la_counts["M_W_fixed"])
# 
#     return (
#         A_W * run.bit_costs["add"]
#         + A_M * run.bit_costs["mul"]
#         + M_W_fixed * mul_const_cost
#     )
# 
# 
# def print_data_dependent_mul_const_comparison(
#     run: XLRun,
#     pred: Prediction,
#     const: bool,
#     bucket: bool,
# ) -> None:
#     """
#     Print alternative predicted totals using the data-dependent mul_const
#     recovered from the Macaulay multiplication diagnostic.
# 
#     This is diagnostic only and does not replace the main prediction.
#     """
#     if (const and not bucket):
#         pass
#     else:
#         return
# 
# 
#     eff = effective_mul_const_from_macaulay(
#         run,
#         const=const,
#         bucket=bucket,
#     )
# 
#     if eff is None:
#         return
# 
#     alt_breakdown = bit_cost_prediction_with_mul_const(
#         pred,
#         run,
#         mul_const_cost=eff,
#     )
# 
#     alt_total = alt_breakdown["total"]
#     alt_bm = predicted_bm_bits_with_mul_const(
#         run,
#         mul_const_cost=eff,
#     )
#     alt_la = predicted_la_bits_with_mul_const(
#         run,
#         const=const,
#         bucket=bucket,
#         mul_const_cost=eff,
#     )
# 
#     print()
#     print("Data-dependent mul_const diagnostic")
#     print("===================================")
#     print(f"{'nominal mul_const':>28}: {fmt_intlike(run.bit_costs['mul_const'])}")
#     print(f"{'Macaulay-derived mul_const':>28}: {fmt_intlike(eff)}")
# 
#     print()
#     print(f"{'quantity':35} {'predicted':>18} {'measured':>18} {'ratio':>12} {'rel err':>12}")
#     print("-" * 100)
# 
#     rows = [
#         ("program total bit ops", alt_total, run.measured_bit_ops),
#         ("program BM bit ops", alt_bm, run.measured_bm_bit_ops),
#         ("program LA bit ops", alt_la, run.measured_la_bit_ops),
#     ]
# 
#     for name, p, m in rows:
#         print(
#             f"{name:35} "
#             f"{fmt_intlike(float(p)):>18} "
#             f"{fmt_intlike(float(m)):>18} "
#             f"{ratio(float(p), float(m)):>12.6f} "
#             f"{rel_error(float(p), float(m)):>12.6%}"
#         )
# 
# def avg_mul_cost_gt1(run: XLRun) -> Optional[float]:
#     """
#     Average multiplication-chain cost conditioned on the constant being
#     neither 0 nor 1.
# 
#     Assumes run.bit_costs["mul_const"] is the average over all q field values.
#     """
#     qv = run.q
# 
#     if qv in (0, 1, 2):
#         return None
# 
#     nominal = (
#         run.nominal_mul_const_cost
#         if run.nominal_mul_const_cost is not None
#         else run.bit_costs["mul_const"]
#     )
#     return nominal * qv / (qv - 2)
# 
# def bucket_macaulay_bit_ops_from_not_zero(run: XLRun) -> Optional[float]:
#     """
#     Diagnostic-only estimate for one bucket-mode Macaulay multiplication.
# 
#     GF(2):
#       - one addition for each nonzero input entry
#       - no >1 buckets and no constant multiplications
# 
#     q > 2:
#       - one addition into a bucket for each nonzero input entry
#       - one bucket-combination addition for each nonzero non-one bucket
#         per output row: (q-2)*R_q
#       - one multiplication by the bucket constant for each such bucket:
#         (q-2)*R_q
#     """
#     if run.macaulay_not_zero is None:
#         return None
# 
#     qv = run.q
#     R_val, _ = model_R_Z_numeric(run.q, run.n, run.m, run.D)
# 
#     N_nonzero = float(run.macaulay_not_zero)
#     Rf = float(R_val)
#     C_A = float(run.bit_costs["add"])
# 
#     # GF(2): constants are only 0 and 1.
#     # Nonzero means 1, so the cost is just adding selected entries.
#     if qv == 2:
#         return N_nonzero * C_A
# 
#     if qv <= 1:
#         return None
# 
#     C_M_gt1 = avg_mul_cost_gt1(run)
#     if C_M_gt1 is None:
#         return None
# 
#     bucket_count = (qv - 2) * Rf
# 
#     return (
#         N_nonzero * C_A
#         + bucket_count * C_A
#         + bucket_count * C_M_gt1
#     )

# def print_bucket_not_zero_diagnostic(
#     run: XLRun,
#     pred: Prediction,
#     const: bool,
#     bucket: bool,
# ) -> None:
#     if not (const and bucket):
#         return
# 
#     if run.macaulay_not_zero is None:
#         return
# 
#     qv = run.q
#     R_val, _ = model_R_Z_numeric(run.q, run.n, run.m, run.D)
# 
#     one_macaulay = bucket_macaulay_bit_ops_from_not_zero(run)
#     if one_macaulay is None:
#         return
# 
#     alt_la = predicted_la_bits_with_bucket_not_zero(run, const, bucket)
#     if alt_la is None:
#         return
# 
#     bm_counts, alt_bm = predicted_bm_counts_and_bits(run)
# 
#     non_bm_la = non_bm_la_predicted_bits(
#         run,
#         pred,
#         const=const,
#         bucket=bucket,
#     )
# 
#     alt_total = float(alt_bm) + float(alt_la) + float(non_bm_la)
# 
#     N_nonzero = float(run.macaulay_not_zero)
#     C_A = float(run.bit_costs["add"])
# 
#     print()
#     print("Bucket not-zero diagnostic")
#     print("==========================")
#     print(f"{'Macaulay not zero':>30}: {fmt_intlike(N_nonzero)}")
#     print(f"{'input bucket add bits':>30}: {fmt_intlike(N_nonzero * C_A)}")
# 
#     if qv == 2:
#         print(f"{'GF(2) bucket multiplications':>30}: 0")
#         print(f"{'GF(2) bucket-combine adds':>30}: 0")
#     else:
#         bucket_count = (qv - 2) * float(R_val)
#         C_M_gt1 = avg_mul_cost_gt1(run)
# 
#         print(f"{'bucket count (q-2)R_q':>30}: {fmt_intlike(bucket_count)}")
#         print(f"{'nominal mul_const avg over GF(q)':>30}: {fmt_intlike(run.bit_costs['mul_const'])}")
#         print(f"{'avg mul_const over >1':>30}: {fmt_intlike(C_M_gt1)}")
#         print(f"{'bucket-combine add bits':>30}: {fmt_intlike(bucket_count * C_A)}")
#         print(f"{'bucket multiplication bits':>30}: {fmt_intlike(bucket_count * C_M_gt1)}")
# 
#     print(f"{'one Macaulay bucket bits':>30}: {fmt_intlike(one_macaulay)}")
#     print(f"{'non-BM/LA predicted bits':>30}: {fmt_intlike(non_bm_la)}")
# 
#     print()
#     print(f"{'quantity':35} {'predicted':>18} {'measured':>18} {'ratio':>12} {'rel err':>12}")
#     print("-" * 100)
# 
#     rows = [
#         ("program total bit ops", alt_total, run.measured_bit_ops),
#         ("program BM bit ops", alt_bm, run.measured_bm_bit_ops),
#         ("program LA bit ops", alt_la, run.measured_la_bit_ops),
#     ]
# 
#     for name, p, m in rows:
#         print(
#             f"{name:35} "
#             f"{fmt_intlike(float(p)):>18} "
#             f"{fmt_intlike(float(m)):>18} "
#             f"{ratio(float(p), float(m)):>12.6f} "
#             f"{rel_error(float(p), float(m)):>12.6%}"
#         )
# 
# def predicted_la_bits_with_bucket_not_zero(
#     run: XLRun,
#     const: bool,
#     bucket: bool,
# ) -> Optional[float]:
#     """
#     Diagnostic-only LA bit prediction for bucket mode using the concrete
#     Macaulay not-zero count.
# 
#     Assumes the printed 'Macaulay not zero' refers to one Macaulay
#     vector-matrix multiplication, and LA repeats this for (2R_q - 1)
#     iterations, matching the current LA model.
#     """
#     if not (const and bucket):
#         return None
# 
#     one_macaulay = bucket_macaulay_bit_ops_from_not_zero(run)
#     if one_macaulay is None:
#         return None
# 
#     R_val, _ = model_R_Z_numeric(run.q, run.n, run.m, run.D)
# 
#     return float(2 * R_val - 1) * float(one_macaulay)
# 
# 
# # def predicted_total_bits_with_bucket_not_zero(
# #     run: XLRun,
# #     pred: Prediction,
# #     const: bool,
# #     bucket: bool,
# # ) -> Optional[float]:
# #     """
# #     Diagnostic-only total prediction for bucket mode using the concrete
# #     not-zero Macaulay statistic for LA.
# #     """
# #     if not (const and bucket):
# #         return None
# # 
# #     alt_la = predicted_la_bits_with_bucket_not_zero(run, const, bucket)
# #     if alt_la is None:
# #         return None
# # 
# #     bm_counts, alt_bm = predicted_bm_counts_and_bits(run)
# # 
# #     # The total is BM + LA + whatever non-BM/non-LA terms are included in
# #     # pred.bit_ops_total but not in bm/la. If your program total is exactly
# #     # BM + LA, this can simply be:
# #     return float(alt_bm) + float(alt_la)
# 
# def non_bm_la_predicted_bits(run: XLRun, pred: Prediction, const: bool, bucket: bool) -> float:
#     """
#     Predicted part of total bit ops not included in program BM or LA counters.
# 
#     This covers evaluation, normalization, and inversion costs.
#     """
#     _, bm_bits = predicted_bm_counts_and_bits(run)
#     _, la_bits = predicted_la_counts_and_bits(run, const=const, bucket=bucket)
# 
#     return float(pred.bit_ops_total) - float(bm_bits) - float(la_bits)
# 
# def print_bucket_not_zero_diagnostic(
#     run: XLRun,
#     pred: Prediction,
#     const: bool,
#     bucket: bool,
# ) -> None:
#     if not (const and bucket):
#         return
# 
#     if run.macaulay_not_zero is None:
#         return
# 
#     qv = run.q
#     R_val, _ = model_R_Z_numeric(run.q, run.n, run.m, run.D)
# 
#     one_macaulay = bucket_macaulay_bit_ops_from_not_zero(run)
#     if one_macaulay is None:
#         return
# 
#     alt_la = predicted_la_bits_with_bucket_not_zero(run, const, bucket)
#     if alt_la is None:
#         return
# 
#     bm_counts, alt_bm = predicted_bm_counts_and_bits(run)
# 
#     non_bm_la = non_bm_la_predicted_bits(
#         run,
#         pred,
#         const=const,
#         bucket=bucket,
#     )
# 
#     alt_total = float(alt_bm) + float(alt_la) + float(non_bm_la)
# 
#     N_nonzero = float(run.macaulay_not_zero)
#     C_A = float(run.bit_costs["add"])
# 
#     print()
#     print("Bucket not-zero diagnostic")
#     print("==========================")
#     print(f"{'Macaulay not zero':>30}: {fmt_intlike(N_nonzero)}")
#     print(f"{'input bucket add bits':>30}: {fmt_intlike(N_nonzero * C_A)}")
# 
#     if qv == 2:
#         print(f"{'GF(2) bucket multiplications':>30}: 0")
#         print(f"{'GF(2) bucket-combine adds':>30}: 0")
#     else:
#         bucket_count = (qv - 2) * float(R_val)
#         C_M_gt1 = avg_mul_cost_gt1(run)
# 
#         print(f"{'bucket count (q-2)R_q':>30}: {fmt_intlike(bucket_count)}")
#         print(f"{'nominal mul_const avg over GF(q)':>30}: {fmt_intlike(run.bit_costs['mul_const'])}")
#         print(f"{'avg mul_const over >1':>30}: {fmt_intlike(C_M_gt1)}")
#         print(f"{'bucket-combine add bits':>30}: {fmt_intlike(bucket_count * C_A)}")
#         print(f"{'bucket multiplication bits':>30}: {fmt_intlike(bucket_count * C_M_gt1)}")
# 
#     print(f"{'one Macaulay bucket bits':>30}: {fmt_intlike(one_macaulay)}")
#     print(f"{'non-BM/LA predicted bits':>30}: {fmt_intlike(non_bm_la)}")
# 
#     print()
#     print(f"{'quantity':35} {'predicted':>18} {'measured':>18} {'ratio':>12} {'rel err':>12}")
#     print("-" * 100)
# 
#     rows = [
#         ("program total bit ops", alt_total, run.measured_bit_ops),
#         ("program BM bit ops", alt_bm, run.measured_bm_bit_ops),
#         ("program LA bit ops", alt_la, run.measured_la_bit_ops),
#     ]
# 
#     for name, p, m in rows:
#         print(
#             f"{name:35} "
#             f"{fmt_intlike(float(p)):>18} "
#             f"{fmt_intlike(float(m)):>18} "
#             f"{ratio(float(p), float(m)):>12.6f} "
#             f"{rel_error(float(p), float(m)):>12.6%}"
#         )

def print_macaulay_mul_debug(run: XLRun, const: bool, bucket: bool) -> None:
    if run.macaulay_mul_bit_ops is None:
        return

    R_val, Z_val = model_R_Z_numeric(run.q, run.n, run.m, run.D)

    print()
    print("Macaulay multiplication diagnostic")
    print("==================================")
    print(f"reported bit ops          = {fmt_intlike(run.macaulay_mul_bit_ops)}")
    print(f"formula Z_q               = {fmt_intlike(float(Z_val))}")

    if run.macaulay_mul_counts:
        for k, v in run.macaulay_mul_counts.items():
            print(f"{k:>24}: {v}")

    from_counts = macaulay_mul_bit_ops_from_counts(run)
    if from_counts is not None:
        print(f"bit ops from field counts = {fmt_intlike(from_counts)}")

    eff = effective_mul_const_from_macaulay(run, const=const, bucket=bucket)
    if eff is not None:
        print(f"effective mul_const cost  = {fmt_intlike(eff)}")
        print(f"nominal mul_const cost    = {fmt_intlike(run.bit_costs['mul_const'])}")
        print(f"difference                = {fmt_intlike(eff - run.bit_costs['mul_const'])}")

def print_report(run: XLRun, pred: Prediction, const: bool, bucket: bool) -> None:
    print()
    print("Parameters")
    print("==========")
    print(f"q={run.q}, n={run.n}, m={run.m}, D={run.D}, seed={run.seed}")

    R_model, Z_model = model_R_Z_numeric(run.q, run.n, run.m, run.D)
    print(f"formula R_q={fmt_intlike(float(R_model))}, formula Z_q={fmt_intlike(float(Z_model))}")
    print(f"program nrows={run.nrows}, program ncols={run.ncols}")
    print(f"prediction mode: {pred.mode}")

    print_binary_model_debug(run)

    print()
    print("Measured field operation costs")
    print("==============================")
    for key in ["add", "dbl", "sub", "mul", "mul_const", "inv"]:
        print(f"{key:>10}: {run.bit_costs[key]}")

    eff_macaulay = effective_mul_const_from_macaulay(
        run,
        const=const,
        bucket=bucket,
    )
    
    print()
    if eff_macaulay is not None:
        print(f"{'mul_const from Macaulay':>20}: {fmt_intlike(eff_macaulay)}")

    if run.use_macaulay_runtime_data:
        print(f"{'runtime data enabled':>20}: yes")
        if run.nominal_mul_const_cost is not None:
            print(f"{'nominal mul_const':>20}: {fmt_intlike(run.nominal_mul_const_cost)}")
            print(f"{'used mul_const':>20}: {fmt_intlike(run.bit_costs['mul_const'])}")
        if run.bucket_A_W_from_data is not None:
            print(f"{'nominal bucket A_W':>20}: {fmt_intlike(run.bucket_A_W_nominal)}")
            print(f"{'runtime bucket A_W':>20}: {fmt_intlike(run.bucket_A_W_from_data)}")

    macaulay_from_counts = macaulay_mul_bit_ops_from_counts(run)
    if macaulay_from_counts is not None:
        print(f"{'Macaulay bits from counts':>20}: {fmt_intlike(macaulay_from_counts)}")
        print(f"{'Macaulay bits reported':>20}: {fmt_intlike(run.macaulay_mul_bit_ops)}")
        print(f"{'Macaulay not zero reported':>20}: {fmt_intlike(run.macaulay_not_zero)}")

    print()
    print("Measured operation counts")
    print("=========================")
    print("GF:")
    for key, value in run.gf_counts.items():
        print(f"  {key:>12}: {value}")
    print(f"  {'field mixed mul':>12}: {run.field_mixed_mul}")

    print()
    print("Predicted operation counts")
    print("==========================")
    print(f"{'field add A':>16}: {fmt_intlike(pred.A)}")
    print(f"{'field sub S':>16}: {fmt_intlike(pred.S)}")
    print(f"{'field mul M':>16}: {fmt_intlike(pred.M)}")
    print(f"{'field inv I':>16}: {fmt_intlike(pred.I)}")
    print(f"{'const mul':>16}: {fmt_intlike(pred.M_fixed)}")

    print()
    print("Predicted bit-op breakdown")
    print("==========================")
    assert pred.bit_ops_breakdown is not None
    for key, value in pred.bit_ops_breakdown.items():
        print(f"{key:>10}: {fmt_intlike(value)}")

    bm_counts, bm_pred_bit_ops = predicted_bm_counts_and_bits(run)
    la_counts, la_pred_bit_ops = predicted_la_counts_and_bits(
        run,
        const=const,
        bucket=bucket,
    )

    print()

    print("BM predicted operation counts")
    print("=============================")
    print(f"{'A_BM':>10}: {fmt_intlike(float(bm_counts['A_BM']))}")
    print(f"{'S_BM':>10}: {fmt_intlike(float(bm_counts['S_BM']))}")
    print(f"{'M_BM':>10}: {fmt_intlike(float(bm_counts['M_BM']))}")
    print(f"{'BM extra bits':>15}: {fmt_intlike(float(bm_counts['BM_extra']))}")
    print()

    print("LA predicted operation counts")
    print("=============================")
    print(f"{'A_W':>12}: {fmt_intlike(float(la_counts['A_W']))}")
    print(f"{'A_M':>12}: {fmt_intlike(float(la_counts['A_M']))}")
    print(f"{'M_W_fixed':>12}: {fmt_intlike(float(la_counts['M_W_fixed']))}")
    print()

    print_macaulay_mul_debug(run, const=const, bucket=bucket)

#    if not run.use_macaulay_runtime_data:
#        print_data_dependent_mul_const_comparison(
#            run,
#            pred,
#            const=const,
#            bucket=bucket,
#        )
#
#        print_bucket_not_zero_diagnostic(
#            run,
#            pred,
#            const=const,
#            bucket=bucket,
#        )


    print()
    print("Comparison")
    print("==========")
    rows = [
        ("program total bit ops", pred.bit_ops_total, run.measured_bit_ops),
        ("program BM bit ops", bm_pred_bit_ops, run.measured_bm_bit_ops),
        ("program LA bit ops", la_pred_bit_ops, run.measured_la_bit_ops),
    ]

    print(f"{'quantity':35} {'predicted':>18} {'measured':>18} {'ratio':>12} {'rel err':>12}")
    print("-" * 100)
    for name, predicted, measured in rows:
        if predicted is None:
            print(f"{name:35} {'-':>18} {fmt_intlike(measured):>18} {'-':>12} {'-':>12}")
        else:
            print(
                f"{name:35} "
                f"{fmt_intlike(predicted):>18} "
                f"{fmt_intlike(measured):>18} "
                f"{ratio(predicted, measured):>12.6f} "
                f"{rel_error(predicted, measured):>12.6%}"
            )

    print()
    print("Notes")
    print("=====")
    print("S means subtraction.  The formula predicts add, sub, mul, inv, and const/mixed mul.")
    print("The counter-derived measured bit-op check also includes dbl counts reported by XL-test.")


def fmt_pred_value(x):
    """
    Format prediction values for wrapper/pred output.

    Predictions are sometimes Sage/RR/float values because of averaged
    mul_const costs. For wrapper-style output, print rounded integers.
    """
    try:
        return str(ZZ(round(RR(x))))
    except Exception:
        return str(int(round(float(x))))


def fmt_measured_value(x):
    """
    Measured XL-test counters should already be integers.
    """
    try:
        return str(ZZ(x))
    except Exception:
        return str(int(x))

def print_wrapper_prediction(run: XLRun, pred: Prediction, const: bool, bucket: bool) -> None:
    """
    Compact wrapper-mode prediction output.

    The raw XL-test output is printed separately. This appends measured values
    together with predictions in a machine-readable-ish format:

        GF:
        field add: ... (pred ...)
        ...
        bit ops: ... (pred ...)
        BM bit ops: ... (pred ...)
        LA bit ops: ... (pred ...)
    """
    bm_counts, bm_pred_bit_ops = predicted_bm_counts_and_bits(run)
    la_counts, la_pred_bit_ops = predicted_la_counts_and_bits(
        run,
        const=const,
        bucket=bucket,
    )

    def measured_gf(name, default=0):
        return run.gf_counts.get(name, default)

    def print_count(name, measured, predicted):
        print(
            f"{name}: {fmt_measured_value(measured)} "
            f"(pred {fmt_pred_value(predicted)})"
        )

    print()
    print("GF:")
    print_count("field add", measured_gf("field add"), pred.A)

    # There is currently no independent symbolic formula for dbl in the
    # Sage model. In wrapper mode we echo the measured dbl count as its
    # prediction so that the bit-cost decomposition remains explicit.
    print_count("field dbl", measured_gf("field dbl"), measured_gf("field dbl"))

    print_count("field sub", measured_gf("field sub"), pred.S)
    print_count("field mul", measured_gf("field mul"), pred.M)
    print_count("field inv", measured_gf("field inv"), pred.I)

    # Optional: useful for const/const+bucket modes. Remove this block if you
    # want the wrapper output to contain only the five GF lines above.
    if run.field_mixed_mul != 0 or pred.M_fixed != 0:
        print_count("field mixed mul", run.field_mixed_mul, pred.M_fixed)

    print()
    print_count("bit ops", run.measured_bit_ops, pred.bit_ops_total)
    print_count("BM bit ops", run.measured_bm_bit_ops, bm_pred_bit_ops)
    print_count("LA bit ops", run.measured_la_bit_ops, la_pred_bit_ops)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main() -> None:
    ap = argparse.ArgumentParser(
        description="Run XL-test and compare measured costs to Sage prediction formulas."
    )

    ap.add_argument("--exe", default="./XL-test", help="Path to XL-test executable")
    ap.add_argument("-q", "--field", type=int, required=True)
    ap.add_argument("-n", type=int, required=True)
    ap.add_argument("-m", type=int, required=True)
    ap.add_argument("-s", "--seed", type=int, default=0)
    ap.add_argument("--perm", type=int)

    ap.add_argument("-c", "--const", action="store_true")
    ap.add_argument("-b", "--bucket", action="store_true")
    ap.add_argument("-a", "--avg", action="store_true")
    ap.add_argument("-e", "--exact", action="store_true")
    ap.add_argument("--trace", action="store_true")

    ap.add_argument("--show-raw", action="store_true", help="Print raw XL-test output before the report")
    ap.add_argument("--print-op-formulas", action="store_true", help="Print A, S, M, I, M_fixed formulas with symbolic R_q and Z_q")
    ap.add_argument("--print-total-bit-formula", action="store_true", help="Print total bit-operation formula")
    ap.add_argument("--export-formula-tex", metavar="DIR", help="Export LaTeX snippets for operation and bit-cost formulas")
    ap.add_argument(
        "--formula-transform",
        choices=["none", "factor", "expand", "simplify"],
        default="none",
        help="Transformation to apply before printing/exporting formulas",
    )
    ap.add_argument(
        "--specialize-bit-formulas",
        action="store_true",
        help="Substitute R_q and Z_q in bit-cost formulas by field-specific R/Z expressions",
    )
    ap.add_argument(
        "--symbolic-cost-factors",
        action="store_true",
        help="Keep C_A, C_S, C_M, C_I, C_M_fixed symbolic instead of using measured costs",
    )
    ap.add_argument(
        "--use-macaulay-runtime-data",
        action="store_true",
        help=(
            "Use Macaulay runtime diagnostics in the regular prediction. "
            "For --const, replace c_mul_const by Macaulay_mul_bit_ops/Z_q. "
            "For --const --bucket, replace the expected nonzero-entry A_W "
            "term by 'Macaulay not zero' and use the >1-conditioned "
            "const-multiplication cost."
        ),
    )
    ap.add_argument(
        "--wrapper",
        action="store_true",
        help=(
            "Wrapper mode: echo XL-test output verbatim, then append compact "
            "prediction lines in the same style as XL-test counters."
        ),
    )
    ap.add_argument(
        "--pred",
        action="store_true",
        help=(
            "Prediction-only mode: run XL-test with '-q FIELD --f-cost' to get "
            "primitive bit costs, then print only predicted counters with "
            "measured values set to 0."
        ),
    )

    # Backward-compatible aliases.
    ap.add_argument("--print-formulas", action="store_true", help=argparse.SUPPRESS)
    ap.add_argument("--factor-formulas", action="store_true", help=argparse.SUPPRESS)
    ap.add_argument("--expand-formulas", action="store_true", help=argparse.SUPPRESS)

    args = ap.parse_args()

    if args.wrapper and args.pred:
        raise SystemExit("Use only one of --wrapper and --pred.")

    if args.pred and args.use_macaulay_runtime_data:
        raise SystemExit(
            "--pred cannot be combined with --use-macaulay-runtime-data, "
            "because XL-test --f-cost does not produce Macaulay runtime diagnostics."
        )

    if args.pred:
        raw_f_cost = run_xl_test_f_cost(args.exe, args.n, args.m, args.field)
        bit_costs = parse_f_cost_output(raw_f_cost)
    
        run = make_prediction_only_run(args, bit_costs)

        pred = select_prediction(
            run,
            const=args.const,
            bucket=args.bucket,
        )
    
        pred = attach_bit_cost_prediction(pred, run)

        print(raw_f_cost, end="" if raw_f_cost.endswith("\n") else "\n")
    
        print_wrapper_prediction(
            run,
            pred,
            const=args.const,
            bucket=args.bucket,
        )
        return

    raw = run_xl_test(
        exe=args.exe,
        q=args.field,
        n=args.n,
        m=args.m,
        seed=args.seed,
        perm=args.perm,
        const=args.const,
        bucket=args.bucket,
        exact_const=args.exact,
        avg_const=args.avg,
        trace=args.trace,
    )

    run = parse_output(raw)

    if args.use_macaulay_runtime_data:
        apply_runtime_data_for_regular_prediction(
            run,
            const=args.const,
            bucket=args.bucket,
        )

    pred = select_prediction(run, const=args.const, bucket=args.bucket)
    pred = apply_data_dependent_prediction_adjustments(
        pred,
        run,
        const=args.const,
        bucket=args.bucket,
    )
    pred = attach_bit_cost_prediction(pred, run)

    if args.wrapper:
        print(raw, end="" if raw.endswith("\n") else "\n")
        print_wrapper_prediction(
            run,
            pred,
            const=args.const,
            bucket=args.bucket,
        )
        return

    if args.show_raw:
        print(raw)

    transform_name = args.formula_transform
    if args.factor_formulas:
        transform_name = "factor"
    elif args.expand_formulas:
        transform_name = "expand"
    transform = choose_transform(transform_name)

    keep_RZ = not args.specialize_bit_formulas
    concrete_costs = not args.symbolic_cost_factors

    if args.print_op_formulas or args.print_formulas:
        print_operation_formulas_generic(pred, transform_name=transform_name)

    if args.print_total_bit_formula:
        print_total_bit_formula(
            pred,
            run,
            const=args.const,
            bucket=args.bucket,
            transform_name=transform_name,
            keep_RZ=keep_RZ,
            concrete_costs=concrete_costs,
        )

    if args.export_formula_tex:
        export_formula_tex_files(
            args.export_formula_tex,
            run,
            transform=transform,
            keep_RZ=keep_RZ,
            concrete_costs=concrete_costs,
        )
        print(f"Wrote formula LaTeX files to {args.export_formula_tex}")

    print_report(run, pred, const=args.const, bucket=args.bucket)


if __name__ == "__main__" and not globals().get("XL_COST_COMPARE_LIBRARY_MODE", False):
    main()


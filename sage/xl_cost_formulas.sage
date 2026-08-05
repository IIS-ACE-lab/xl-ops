# xl_cost_formulas.sage

var('R_q Z_q m n d q')
var('C_A C_S C_M C_I C_M_fixed')

# --------------------------------------------------------------------
# Generic operation-count formulas in terms of R_q, Z_q
# --------------------------------------------------------------------

A_BM = 3*R_q*(R_q + 1)/2
S_BM = R_q*(3*R_q + 5)/2
M_BM = 3*R_q*(R_q + 1)/2 + 2 * (R_q*(3*R_q + 5)/2)

norm_M = R_q+1
norm_I = 1

eval_A = R_q*n
eval_M = R_q*n

var('k g')

def bm_extra_bits_symbolic(qq):
#    if qq == 2:
#        return (
#           (0                              # d.not_zero()  => const 0 for GF(2)
#            + 5*k-5                        # bit_vector_gt()
#            + 1                            # cond & gt
#            + k - 1                        # invert L
#            + 5*(k-1)                      # bit_vector_add()
#            + 3*(k-1)                      # bit_vector_mux()
#            + 3*g                          # mux(delta.v,d.v,cond)
#           ) * 2*R_q                       # outer loop runs 2*R_q times
#           + (3*g)                         # mux cost
#             * ((3*R_q + 5)*R_q)/2         # loop executions             
#        )
#
##        return (
##            2*R_q*(14*k - 10)
##            + (9*R_q^2 + 15*R_q)/2
##        )
#    else:
      return (
          (g-1                            # d.not_zero()
           + 5*k-5                        # bit_vector_gt()
           + 1                            # cond & gt
           + k - 1                        # invert L
           + 5*(k-1)                      # bit_vector_add()
           + 3*(k-1)                      # bit_vector_mux()
           + 3*g                          # mux(delta.v,d.v,cond)
          ) * 2*R_q                       # outer loop runs 2*R_q times
          + (3*g)                         # mux cost
            * ((3*R_q + 5)*R_q)/2         # loop executions             
      )

#    return (
#        28*R_q*k
#        + 8*R_q*g
#        - 26*R_q
#        + (9*g*R_q^2 + 15*g*R_q)/2
#    )

def generic_baseline_formulas():
    A = (2*R_q - 1)*Z_q
    M = (2*R_q - 1)*Z_q
    M_fixed = 0

    return {
        "A_W": A,
        "M_W": M,
        "M_fixed" : M_fixed,
        "A": A + A_BM + eval_A,
        "S": S_BM,
        "M": M + M_BM + norm_M + eval_M,
        "I": norm_I,
    }


def generic_const_formulas():
    A = (2*R_q - 1)*Z_q
    M = 0
    M_fixed = (2*R_q - 1)*Z_q

    return {
        "A_W": A,
        "M_W": M,
        "M_fixed" : M_fixed,
        "A": A + A_BM + eval_A,
        "S": S_BM,
        "M": M_BM + norm_M + eval_M,
        "I": norm_I,
    }


def generic_const_bucket_formulas():
    A = (2*R_q - 1)*(((q - 1)/q)*Z_q + (q - 2)*R_q)
    M = 0
    M_fixed = (2*R_q - 1)*(q - 2)*R_q

    return {
        "A_W": A,
        "M_W": M,
        "M_fixed" : M_fixed,
        "A": A + A_BM + eval_A,
        "S": S_BM,
        "M": M_BM + norm_M + eval_M,
        "I": norm_I,
    }


def field_bit_width(qq):
    """
    Bit width g of the field representation.

    q=310 is GF(31) without bitops, so use the GF(31) width.
    """
    if qq == 2:
        return ZZ(1)
    if qq == 31 or qq == 310:
        return ZZ(5)
    if qq == 256:
        return ZZ(8)

    # Fallback if you add new fields later.
    return ZZ(ceil(log(ZZ(qq), 2)))


def bm_extra_bits_numeric(qq, R_val):
    """
    Numeric exact non-field BM bit cost, evaluated from the symbolic formula.
    """
    R_val = ZZ(R_val)
    k_val = ZZ(ceil(log(2*R_val, 2) + 1))

    subs = {
        R_q: R_val,
        k: k_val,
        g: field_bit_width(qq),
    }

    if qq != 2:
        subs[g] = field_bit_width(qq)
    else:
        subs[g] = 1

    value = bm_extra_bits_symbolic(qq).subs(subs)

    if hasattr(value, "variables") and len(value.variables()) != 0:
        raise ValueError(
            "BM extra formula did not evaluate numerically.\n"
            f"q={qq}, R={R_val}\n"
            f"value={value}\n"
            f"remaining variables={value.variables()}"
        )

    if value not in ZZ:
        raise ValueError(
            "BM extra formula did not evaluate to an integer.\n"
            f"q={qq}, R={R_val}\n"
            f"value={value}"
        )

    return ZZ(value)


# --------------------------------------------------------------------
# Non-binary R, Z
# --------------------------------------------------------------------

R = binomial(n + d, d) - 1

Z = (
    m*n*(n + 3)/2
    + (R - m)*(n + 1)*(n + 2)/2
)


# --------------------------------------------------------------------
# Binary R_2, Z_2
# --------------------------------------------------------------------

Bfun = function("B")

R2 = Bfun(n, d) - 1

T2_unpruned = (
    Bfun(n, d - 2) - 1
    + n*(Bfun(n - 1, d - 2) - 1)
    + n*(n - 1)/2 * (Bfun(n - 2, d - 2) - 1)
)

T2 = (
    T2_unpruned
    * (R2 - m)
    / (Bfun(n, d - 2) - 1)
)

Z2 = m*(n + n*(n - 1)/2) + T2


# --------------------------------------------------------------------
# Formula selection and specialization
# --------------------------------------------------------------------

def remaining_variables(expr):
    if hasattr(expr, "variables"):
        return expr.variables()
    return ()


def specialize_formulas(formulas, R_expr, Z_expr, q_expr=None):
    subs_dict = {R_q: R_expr, Z_q: Z_expr}
    if q_expr is not None:
        subs_dict[q] = q_expr

    out = {}
    for name, expr in formulas.items():
        out[name] = expr.subs(subs_dict) if hasattr(expr, "subs") else expr
    return out


def select_generic_formulas(const=False, bucket=False):
    if const and bucket:
        return generic_const_bucket_formulas()
    if const:
        return generic_const_formulas()
    return generic_baseline_formulas()


def select_field_specialized_formulas(qq, const=False, bucket=False):
    generic = select_generic_formulas(const=const, bucket=bucket)
    if qq == 2:
        return specialize_formulas(generic, R2, Z2, 2)
    return specialize_formulas(generic, R, Z, qq)


# # Backward-compatible names.
# def nonbinary_baseline_formulas():
#     return specialize_formulas(generic_baseline_formulas(), R, Z)
# 
# 
# def nonbinary_const_formulas():
#     return specialize_formulas(generic_const_formulas(), R, Z)
# 
# 
# def binary_baseline_formulas():
#     return specialize_formulas(generic_baseline_formulas(), R2, Z2, 2)
# 
# 
# def binary_const_formulas():
#     return specialize_formulas(generic_const_formulas(), R2, Z2, 2)
# 
# 
# def binary_const_bucket_formulas():
#     return specialize_formulas(generic_const_bucket_formulas(), R2, Z2, 2)
# 
# 
# def select_symbolic_formulas(qq, const=False, bucket=False):
#     return select_field_specialized_formulas(qq, const=const, bucket=bucket)


# --------------------------------------------------------------------
# Numeric B_d(n) support for q == 2
# --------------------------------------------------------------------

def B_squarefree_numeric(nn, dd):
    nn = ZZ(nn)
    dd = ZZ(dd)
    if dd < 0:
        return ZZ(0)
    return sum(binomial(nn, k) for k in range(dd + 1))


def numeric_subs_dict(qq, nn, mm, dd):
    return {q: ZZ(qq), n: ZZ(nn), m: ZZ(mm), d: ZZ(dd)}


def binary_B_numeric_subs_after_nd(qq, nn, mm, dd):
    if qq != 2:
        return {}

    nn = ZZ(nn)
    dd = ZZ(dd)
    return {
        Bfun(nn, dd): B_squarefree_numeric(nn, dd),
        Bfun(nn, dd - 2): B_squarefree_numeric(nn, dd - 2),
        Bfun(nn - 1, dd - 2): B_squarefree_numeric(nn - 1, dd - 2),
        Bfun(nn - 2, dd - 2): B_squarefree_numeric(nn - 2, dd - 2),
    }


#def evaluate_formula_dict(formulas, qq, nn, mm, dd):
#    base_subs = numeric_subs_dict(qq, nn, mm, dd)
#    B_subs = binary_B_numeric_subs_after_nd(qq, nn, mm, dd)
#
#    out = {}
#    for name, expr in formulas.items():
#        e = expr.subs(base_subs) if hasattr(expr, "subs") else expr
#        if qq == 2 and hasattr(e, "subs"):
#            e = e.subs(B_subs)
#
#        vars_left = remaining_variables(e)
#        if len(vars_left) != 0:
#            raise ValueError(
#                "Formula did not evaluate to a numeric expression.\n"
#                f"name: {name}\n"
#                f"expression after substitution: {e}\n"
#                f"remaining variables: {vars_left}"
#            )
#        out[name] = RR(e)
#    return out


def model_R_Z_numeric(qq, nn, mm, dd):
    base_subs = numeric_subs_dict(qq, nn, mm, dd)

    if qq == 2:
        B_subs = binary_B_numeric_subs_after_nd(qq, nn, mm, dd)
        R_val = R2.subs(base_subs).subs(B_subs)
        Z_val = Z2.subs(base_subs).subs(B_subs)
    else:
        R_val = R.subs(base_subs)
        Z_val = Z.subs(base_subs)

    R_vars = remaining_variables(R_val)
    Z_vars = remaining_variables(Z_val)
    if len(R_vars) != 0 or len(Z_vars) != 0:
        raise ValueError(
            "R_q or Z_q did not evaluate numerically.\n"
            f"R_val: {R_val}\n"
            f"Z_val: {Z_val}\n"
            f"R remaining variables: {R_vars}\n"
            f"Z remaining variables: {Z_vars}"
        )

    return QQ(R_val), QQ(Z_val)


# --------------------------------------------------------------------
# Total bit-cost formulas
# --------------------------------------------------------------------

def total_bit_formula(formulas, qq=None, include_bm_extra=True):
    total = (
        C_A * formulas.get("A", 0)
        + C_S * formulas.get("S", 0)
        + C_M * formulas.get("M", 0)
        + C_I * formulas.get("I", 0)
        + C_M_fixed * formulas.get("M_fixed", 0)
    )

    if include_bm_extra and qq is not None:
        total += bm_extra_bits_symbolic(qq)

    return total

#def total_bit_formula(formulas):
#    if formulas is None:
#        raise RuntimeError("total_bit_formula received formulas=None")
#
#    return (
#        C_A * formulas.get("A", 0)
#        + C_S * formulas.get("S", 0)
#        + C_M * formulas.get("M", 0)
#        + C_I * formulas.get("I", 0)
#        + C_M_fixed * formulas.get("M_fixed", 0)
#    )


#def select_total_bit_formula_generic(const=False, bucket=False):
#    return total_bit_formula(select_generic_formulas(const=const, bucket=bucket))


#def select_total_bit_formula_specialized(qq, const=False, bucket=False):
#    return total_bit_formula(
#        select_field_specialized_formulas(qq, const=const, bucket=bucket)
#    )


## Backward-compatible name.
#def select_total_bit_formula(qq, const=False, bucket=False):
#    return select_total_bit_formula_specialized(qq, const=const, bucket=bucket)


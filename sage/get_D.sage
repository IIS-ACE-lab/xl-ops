#!/usr/bin/env sage

import argparse

def ps(q, m, n, prec=20):
  R.<X> = PowerSeriesRing(ZZ, default_prec=prec)

  if(q == 2):
    return (1+X)^n/((1-X)*(1+X^2)^m)
  else:
    return (1-X)^(m-n-1)*(1+X)^m


def deg_info(q, m, n):
  prec=20

  while True:
    f_XL = ps(q, m, n, prec)

    for i, coef in enumerate(f_XL):
      if coef <= 0:
        return i

    prec = 2*prec

  return -1


parser = argparse.ArgumentParser(description='',
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-q', dest='q',
                   type=int, required=True,
                   help='field')

parser.add_argument('-m', dest='m',
                   type=int, required=True,
                   help='M')

parser.add_argument('-n', dest='n',
                   type=int, required=True,
                   help='N')

parser.add_argument("-s", "--stat", help="print stat", action="store_true")

options = parser.parse_args()

deg = deg_info(q = options.q, m = options.m, n = options.n)

def boolean_monomials(n, D):
    if D < 0:
        return 0
    return sum(binomial(n, k) for k in range(0, min(D, n) + 1))

def gf2_macaulay_shape(n, D, m):
    rows = m * boolean_monomials(n, D - 2)
    cols = boolean_monomials(n, D)
    return rows, cols

def gf2_boolean_potential_entries(n, D, m):
    total = 0

    max_r = min(D - 2, n)

    for r in range(max_r + 1):
        num_multipliers = binomial(n, r)

        max_j = min(2, D - r, n - r)
        touched_cols_per_row = sum(binomial(n - r, j) for j in range(max_j + 1))

        total += num_multipliers * touched_cols_per_row

    return m * total


if not options.stat:
  print(deg)
else:

  def num_mons_sum(q, n, d):
      if(q == 2):
          sum = 0;
          for i in range(d + 1):
              sum += binomial(n, i);
      else:
          sum = binomial(n + d, d);
  
      return sum;

 
  n = options.n
  m = options.m
  
  Mac_width = num_mons_sum(options.q, n, deg)
  orig_sys_m = num_mons_sum(options.q, n, 2)

  num_it = 2*Mac_width

  bm = (num_it*num_it) / 2.0
  
  if (options.q == 2):
    rows, cols = gf2_macaulay_shape(n, deg, m)
#    print(rows, cols, cols / rows)
    entries = gf2_boolean_potential_entries(n, deg, m) / rows * cols
#    print(entries)
    bw1 = entries * num_it
  else:
    bw1 = (Mac_width * orig_sys_m) * num_it

#  print((Mac_width * orig_sys_m))
  
  print("%i %.0f %.0f %.02f %i %i %i" % (m, bw1, bm, bw1/bm, Mac_width, orig_sys_m, deg))


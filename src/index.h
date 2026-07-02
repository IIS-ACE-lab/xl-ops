/*
   This file is part of the CryptAttackTester by Daniel J. Bernstein and Tung Chou.

   Original source:
     https://cat.cr.yp.to/cryptattacktester-20231020.tar.gz

*/

#ifndef index_h
#define index_h

#include <vector>
#include "bigint.h"
#include "bit.h"

static inline void half_adder(bit &s, bit &c, bit a, bit b)
{
        s = a ^ b;
        c = a & b;
}

static inline void full_adder(bit &s, bit &c, bit a, bit b)
{
        bit t = (a ^ b);

        s = t ^ c;
        c = (a & b) | (c & t);
}

bit bit_vector_gt(std::vector<bit> &,std::vector<bit> &);
bit bit_vector_gt_rev(std::vector<bit> &,std::vector<bit> &);
std::vector<bit> bit_vector_add(std::vector<bit> &, std::vector<bit>, std::vector<bit> b, bit = bit(0));
std::vector<bit> bit_vector_hamming_weight(const std::vector<bit> &);
bit bit_vector_hamming_weight_isnot(const std::vector<bit> &,bigint);

std::vector<bit> indices_to_vector(std::vector<std::vector<bit>> &, bigint);
std::vector<bit> set_size(std::vector<std::vector<bit>> &);
bit set_size_check(std::vector<std::vector<bit>> &, bigint);

bigint index_value(const std::vector<bit> &);

#endif

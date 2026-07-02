#include <iostream>
#include <getopt.h>
#include <math.h>

#include "bit.h"
#include "bit_vector.h"
#include "index.h"

#include <cassert>

//reduction
vector<bit> gf31_reduction(vector<bit> res_in)
{
    vector<bit> abit = bit_vector_from_integer(0, 5);
    vector<bit> bbit = bit_vector_from_integer(0, 5);
    vector<bit> res = bit_vector_from_integer(0, 5);

    for (unsigned int i = 0; i < 5; i++)
    {
        abit.at(i) = res_in.at(i);
    }

    for (unsigned int i = 5; i < res_in.size(); i++)
    {
        int j = i - 5;
        bbit.at(j) = res_in.at(i);
    }

    bit carry = 0;

    half_adder(res.at(0), carry, abit.at(0), bbit.at(0));

    for (unsigned int i = 1; i < abit.size(); i++)
       full_adder(res.at(i), carry, abit.at(i), bbit.at(i));

    for (unsigned int i = 0; i < res.size(); i++)
       half_adder(res.at(i), carry, res.at(i), carry);

    return res;
}

//addition for GF(31)
vector<bit> gf31_add(const vector<bit> &a, const vector<bit> &b)
{
    assert(a.size() == b.size());

    vector<bit> sum = bit_vector_from_integer(0, a.size());

    bit carry = 0;

    half_adder(sum.at(0), carry, a.at(0), b.at(0));

    for (unsigned int i = 1; i < a.size(); i++)
       full_adder(sum.at(i), carry, a.at(i), b.at(i));

    // reduce carry
    for (unsigned int i = 0; i < sum.size(); i++)
       half_adder(sum.at(i), carry, sum.at(i), carry);

    // 31 maps to 0
    bit cond = sum.at(0);

    for (unsigned int i = 1; i < sum.size(); i++)
       cond = cond & sum.at(i);

    for (unsigned int i = 0; i < sum.size(); i++)
       sum.at(i) = sum.at(i) ^ cond;

    return sum;
}

vector<bit> gf31_dbl(const vector<bit>& a)
{
    vector<bit> res(a.size() + 1, bit(0));

    for (int i = 0; i < (int)a.size(); i++)
    {
        res.at(i + 1) = a.at(i).value();
    }

    res = gf31_reduction(res);

    return res;
}


//addition of integers
vector<bit> int_add(const vector<bit> &a, const vector<bit> &b)
{
   assert(a.size() == b.size());

   bit carry = 0;
   vector<bit> sum = bit_vector_from_integer(0, a.size());

    for (unsigned int i = 0; i < a.size(); i++)
       full_adder(sum.at(i), carry, i < a.size() ? a.at(i) : 0, i < b.size() ? b.at(i) : 0);

   sum.insert(sum.end(), carry);

   return sum;
}

static inline void full_adder_no_carry(bit &s, bit &c, bit a, bit b)
{
        bit t = (a ^ b);
        s = t ^ c;
}

//negative of an integer
vector<bit> int_neg(const vector<bit> &a)
{
    vector<bit> neg = bit_vector_from_integer(0, a.size()+1);

    for (unsigned int i = 0; i < a.size(); i++)
      neg.at(i) = a.at(i);

    for (unsigned int i = 0; i < neg.size(); i++)
      neg.at(i) = ~neg.at(i);

    bit c = 0;
    vector<bit> carry = bit_vector_from_integer(1, a.size()+1);

    unsigned int i;

    for(i = 0; i < neg.size() - 1; i++)
       full_adder(neg.at(i), c, neg.at(i), carry.at(i));

    full_adder_no_carry(neg.at(i), c, neg.at(i), carry.at(i));

    return neg;
}

//addition of integers discarding carry
vector<bit> int_add_trunc(const vector<bit> &a, const vector<bit> &b)
{
   assert(a.size() == b.size());

   bit carry = 0;
   vector<bit> sum = bit_vector_from_integer(0, a.size());

   unsigned int i;

   for (i = 0; i < a.size()-1; i++)
      full_adder(sum.at(i), carry, i < a.size() ? a.at(i) : 0, i < b.size() ? b.at(i) : 0);

   full_adder_no_carry(sum.at(i), carry, i < a.size() ? a.at(i) : 0, i < b.size() ? b.at(i) : 0);

   return sum;
}


//substruction for GF(31)
vector<bit> gf31_sub(const vector<bit> &a, const vector<bit> &b)
{
   assert((a.size() == 5) && (b.size() == 5));

   vector<bit> b_neg(b.size());

   // For Mersenne prime fields, we get the negative as bit-wise not (with special case 0).
   for (unsigned int i = 0; i < b.size(); i++)
     b_neg.at(i) = ~b.at(i);

   return gf31_add(a, b_neg);
}

//multiplication reduce at the end for GF(31)
vector<bit> gf31_mul(const vector<bit> &a, const vector<bit> &b)
{
    assert(a.size() == b.size());

    vector<bit> addbit = bit_vector_from_integer(0, a.size());

    for (unsigned int i = 0; i < a.size(); i++)
        addbit.at(i) = a.at(i) & b.at(0);

    for (unsigned int i = 1; i < b.size(); i++)
    {
        vector<bit> mulbit = bit_vector_from_integer(0, a.size());

        for (unsigned int j = 0; j < a.size(); j++)
            mulbit.at(j) = a.at(j) & b.at(i);
        
        for (unsigned int t = 0; t < i; t++)
            mulbit.insert(mulbit.begin(), 0);

        int sizeadd = addbit.size();
        int sizemul = mulbit.size();

        for (int t = 0; t < sizemul-sizeadd; t++)
            addbit.insert(addbit.end(), 0);

        addbit = int_add(addbit, mulbit);
    }

    vector<bit> res = gf31_reduction(addbit);

    return res;
}

//multiplication add and reduce for GF(31)
vector<bit> gf31_muladd(const vector<bit> &a, const vector<bit> &b)
{
    assert(a.size() == b.size());

    vector<bit> addbit = bit_vector_from_integer(0, a.size());

    for (unsigned int i = 0; i < a.size(); i++)
        addbit.at(i) = a.at(i) & b.at(0);

    for (unsigned int i = 1; i < b.size(); i++)
    {
        vector<bit> mulbit = bit_vector_from_integer(0, a.size());

        for (unsigned int j = 0; j < a.size(); j++)
            mulbit.at(j) = a.at(j) & b.at(i);

        for (unsigned int t = 0; t < i; t++)
            mulbit.insert(mulbit.begin(), 0);

        mulbit = gf31_reduction(mulbit);
        addbit = int_add(addbit, mulbit);
        addbit = gf31_reduction(addbit);
    }

    return addbit;
}


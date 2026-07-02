/*
   This file is part of the CryptAttackTester by Daniel J. Bernstein and Tung Chou.

   Original source:
     https://cat.cr.yp.to/cryptattacktester-20231020.tar.gz

*/

#include "bit_vector.h"
#include "index.h"
//#include "sorting.h"

using namespace std;

bigint index_value(const vector<bit> &v)
{
	bigint ret = 0;

	for (bigint i = 0; i < v.size(); i++)
		ret += v.at(i).value() << i; // XXX: takes only bottom bit from bitset

	return ret;
}

bit bit_vector_gt(vector<bit> &v,vector<bit> &w)
{
	bigint len = v.size();
	assert(len == w.size());

	bit ret;
	if (len <= 0) return ret;

	bigint i = len-1;
        ret = v.at(i).andn(w.at(i));
        if (i == 0) return ret;
        bit d = v.at(i) ^ w.at(i);

        for (;;) {
          --i;
          ret |= v.at(i).andn(w.at(i)).andn(d);
          if (i == 0) return ret;
          d |= v.at(i) ^ w.at(i);
        }
}

bit bit_vector_gt_rev(vector<bit> &v,vector<bit> &w)
{
	bigint len = v.size();
	assert(len == w.size());

	bit ret;
	if (len <= 0) return ret;

	bigint i = 0;
        ret = v.at(i).andn(w.at(i));
        if (i == len-1) return ret;
        bit d = v.at(i) ^ w.at(i);

        for (;;) {
          ++i;
          ret |= v.at(i).andn(w.at(i)).andn(d);
          if (i == len-1) return ret;
          d |= v.at(i) ^ w.at(i);
        }
}

vector<bit> bit_vector_add(vector<bit> &ret, vector<bit> a, vector<bit> b, bit c_in)
{
        if (b.size() > a.size())
        	return bit_vector_add(ret,b,a,c_in);

        assert(a.size()+1 >= ret.size());
        assert(ret.size() >= a.size());
        assert(a.size() >= b.size());

        bit c = c_in;
        for (bigint i = 0; i < b.size(); i++)
                full_adder(ret.at(i), c, a.at(i), b.at(i));

        for (bigint i = b.size(); i < a.size(); i++)
                half_adder(ret.at(i), c, a.at(i), c);

        if (a.size() < ret.size())
                ret.at(a.size()) = c;

        return ret;
}

vector<bit> bit_vector_hamming_weight(const vector<bit> &v)
{
        bigint n = v.size();
        bigint k = nbits(n) - 1;

        if (n == 0) return vector<bit> (0);
        if (n == 1) return vector<bit> (1, v.at(0));

        bigint x = 1;

        x <<= k;
        x -= 1;

        bigint y = n - x - 1;

        assert(n == x + y + 1);

        vector<bit> weight_x = bit_vector_hamming_weight(bit_vector_extract(v, 0, x));
        vector<bit> weight_y = bit_vector_hamming_weight(bit_vector_extract(v, x, x + y));

        vector<bit> ret(nbits(n));

        bit_vector_add(ret, weight_x, weight_y, v.back());

        return ret;
}

bit bit_vector_hamming_weight_isnot(const vector<bit> &v,bigint target)
{
  vector<bit> w = bit_vector_hamming_weight(v);
  assert(nbits(target) <= w.size());
  bit ret;
  // XXX: can speed up i=0
  for (bigint i = 0;i < w.size();++i)
    if (target.bit(i))
      ret = ret.orn(w.at(i));
    else
      ret |= w.at(i);
  return ret;
}

//// merge with bit_matrix_sum_of_cols_viasorting
//// note that for each i in indices, e[i] = 0 instead of 1
//vector<bit> indices_to_vector(vector<vector<bit>> &indices, bigint n)
//{
//        vector<bit> e(0);
//        bigint idx_size = indices.at(0).size();
//
//        vector<vector<bit>> L;
//
//        for (bigint i = 0; i < indices.size(); i++)
//        {
//                bigint j = indices.size()-1-i;
//
//                vector<bit> v_j = bit_vector_from_integer(j, idx_size, 1);
//                vector<bit> idx = indices.at(i);
//                vector<bit> v(idx_size);
//
//                bit_vector_add(v, v_j, idx, bit(1));
//
//                v.insert(v.begin(), bit(0));
//                L.push_back(v);
//        }
//
//        for (bigint i = 0; i < n; i++)
//        {
//                vector<bit> v = bit_vector_from_integer(i, idx_size);
//                v.insert(v.begin(), bit(1));
//                L.push_back(v);
//        }
//
//        sorting(L);
//
//        for (bigint i = 0; i < n; i++)
//                e.push_back(L.at(i).at(0));
//
//        return e;
//}
//
//vector<bit> set_size(vector<vector<bit>> &set)
//{
//        bit b;
//        vector<bit> v(set.size(), bit(1));
//
//        sorting(set);
//
//        for (bigint i = 0; i < set.size()-1; i++)
//        {
//                b = v.at(i+0) & v.at(i+1);
//                b = b.andn(bit_vector_compare(set.at(i+0), set.at(i+1)));
//                v.at(i+0) ^= b;
//                v.at(i+1) ^= b;
//        }
//
//        return bit_vector_hamming_weight(v);
//}
//
//bit set_size_check(vector<vector<bit>> &set, bigint w)
//{
//        vector<bit> w_set = set_size(set);
//        vector<bit> w_vec = bit_vector_from_integer(w);
//
//        bit ret = ~bit_vector_integer_compare(w_set, w_vec);
//
//        return ret;
//}


/*
   This file is part of the CryptAttackTester by Daniel J. Bernstein and Tung Chou.

   Original source:
     https://cat.cr.yp.to/cryptattacktester-20231020.tar.gz

*/

#ifndef UTIL_H
#define UTIL_H

#include <vector>

#include "bigint.h"
#include "bit.h"

using namespace std;

static inline bigint gray_idx(vector<bigint> &q)
{
	bigint idx = 0;

	while(q.size() > 0 and q.back() == idx)
	{
		q.pop_back();
		idx++;
	}

	q.push_back(idx);
	return idx;
}

#endif


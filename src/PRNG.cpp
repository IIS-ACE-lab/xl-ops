#include <math.h>
#include <stdint.h>

#include <iostream>

#include "fips202.h"

using namespace std;



static keccak_state shake;

void random_init(const uint64_t seed)
{
  shake256_absorb_once(&shake, (const unsigned char*)&seed, sizeof(seed));
}

uint64_t random_int(unsigned nbits)
{
   int nbytes = ceil(nbits / 8.0);

   uint64_t ret = 0;
   
   shake256_squeeze((unsigned char*)&ret, nbytes, &shake);

   ret = ret & ((1 << nbits) - 1);

   return ret;
}

uint64_t random_val(unsigned max)
{
   int nbits = ceil(log2(max));
   int nbytes = ceil(nbits / 8.0);

   uint64_t ret = max;

   while (ret >= max)
   {
     shake256_squeeze((unsigned char*)&ret, nbytes, &shake);

     ret = ret & ((1 << nbits) - 1);

     //cout << max << " " << nbits << " " << ret << "\n";
     //cout << nbits << " " << ret << "\n";
   }

   return ret;
}


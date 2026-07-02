#ifndef __PRNG_H
#define __PRNG_H

#include <stdint.h>

void random_init(const uint64_t seed);
uint64_t random_int(unsigned nbits);
uint64_t random_val(unsigned max);

#endif


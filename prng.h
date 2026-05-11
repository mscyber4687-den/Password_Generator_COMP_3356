#ifndef PRNG_H
#define PRNG_H

void prng_seed(unsigned int seed);
unsigned int prng_next(void);
unsigned int lcg(unsigned int seed);

#endif

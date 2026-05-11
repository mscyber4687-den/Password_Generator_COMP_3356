#include "prng.h"

/* Internal PRNG state. */
static unsigned int current_state = 1;

void prng_seed(unsigned int seed) {
    /*
     * Avoid a completely predictable zero-only state pattern
     * if students decide to experiment with other generators later.
     */
    if (seed == 0) {
        seed = 1;
    }
    current_state = seed;
}

unsigned int lcg(unsigned int seed) {

      // a = 1664525, c = 1013904223, m = 2^32

      //Linear Congruential Generator:
      //X(n+1) = (a * X(n) + c) mod 2^32
     
      //Since unsigned int arithmetic wraps modulo 2^32 on typical
      //32-bit arithmetic behavior for unsigned values, this works naturally.
     
    static const unsigned int a = 1664525U;
    static const unsigned int c = 1013904223U;

    return (a * seed) + c;
}

unsigned int prng_next(void) {
    current_state = lcg(current_state);
    return current_state;
}

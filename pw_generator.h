#ifndef PW_GENERATOR_H
#define PW_GENERATOR_H

#include <stddef.h>

int generate_password(char *out_password,
                      size_t out_size,
                      const char *alphabet,
                      size_t alphabet_size,
                      int length,
                      unsigned int *random_numbers,
                      size_t *random_index);

#endif

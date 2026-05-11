#include "pw_generator.h"

#include <limits.h>
#include <stdint.h>
#include <stddef.h>

#include "prng.h"

/*
 * Generates an "unbiased" index using rejection sampling.
 * Reject PRNG values at the top of the 2^32 range that would skew
 * the value % alphabet_size in favor of lower indices.
 */
static unsigned int random_index_in_range(size_t alphabet_size,
                                          unsigned int *raw_value) {
    uint64_t range = (uint64_t)UINT_MAX + 1ULL;
    uint64_t limit = range - (range % alphabet_size);
    unsigned int value;

    do {
        value = prng_next();
    } while ((uint64_t)value >= limit);

    if (raw_value != NULL) {
        *raw_value = value;
    }

    return (unsigned int)((uint64_t)value % alphabet_size);
}
/* Generates a password of the specified length using characters from the provided alphabet.
 * The random_numbers array is filled with the raw PRNG values used for each character selection,
 * and random_index is updated to reflect how many random numbers were used. */
int generate_password(char *out_password,
                      size_t out_size,
                      const char *alphabet,
                      size_t alphabet_size,
                      int length,
                      unsigned int *random_numbers,
                      size_t *random_index) {
    if (out_password == NULL ||
        alphabet == NULL ||
        alphabet_size == 0 ||
        random_numbers == NULL ||
        random_index == NULL ||
        length <= 0 ||
        out_size < (size_t)length + 1) {
        return 0;
    }

    for (int i = 0; i < length; i++) {
        unsigned int raw_value = 0;
        unsigned int index = random_index_in_range(alphabet_size, &raw_value);
        out_password[i] = alphabet[index];
        random_numbers[*random_index] = raw_value;
        (*random_index)++;
    }

    out_password[length] = '\0';
    return 1;
}

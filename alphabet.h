#ifndef ALPHABET_H
#define ALPHABET_H

#include <stddef.h>

#define MAX_ALPHABET_SIZE 128

/*
 * Builds the final alphabet from:
 *   - flags like "-lu", "-d", "-s"
 *   - a custom alphabet string
 *
 * Returns:
 *   >= 0 : number of characters in the final alphabet
 *   -1   : error
 *
 * The output buffer must be large enough to hold the final alphabet
 * plus a null terminator.
 */
int build_alphabet(const char *flags,
                   const char *custom_alphabet,
                   char *out_alphabet,
                   size_t out_size);

#endif

#include "information_content.h"

#include <math.h>

/* Calculates the information content of a password given its length and the size of the alphabet. */
double information_content(int length, size_t alphabet_size) {
    if (length <= 0 || alphabet_size == 0) {
        return 0.0;
    }
    /* The information content is calculated as length * log2(alphabet_size). */
    return (double)length * (log((double)alphabet_size) / log(2.0));
}

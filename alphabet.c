#include "alphabet.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char *LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
static const char *UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char *DIGITS    = "0123456789";
static const char *SYMBOLS   = "~!@#$%^&*()_+-={}[]|:;<>,.?/";

/* Adds characters from src into out_alphabet once, and preserves input order. */
static int append_unique_chars(const char *src,
                               int used[128],
                               char *out_alphabet,
                               size_t out_size,
                               size_t *index) {
    if (src == NULL) {
        return 1;
    }
    /* For each character in src, if it's a valid ASCII character and hasn't been used yet,
     * add it to out_alphabet and mark it as used. */
    for (size_t i = 0; src[i] != '\0'; i++) {
        unsigned char c = (unsigned char)src[i];
        if (c < 128 && !used[c]) {
            if (*index + 1 >= out_size) {
                fprintf(stderr, "Error: alphabet buffer too small.\n");
                return 0;
            }
            used[c] = 1;
            out_alphabet[*index] = (char)c;
            (*index)++;
        }
    }
   
    return 1;
}

/* Validate every character in src is graphical ASCII. */
static int validate_custom_alphabet(const char *src) {
    if (src == NULL) {
        return 1;
    }
    /* For each character in src, if it's a valid ASCII character and is graphical, continue. */
    for (size_t i = 0; src[i] != '\0'; i++) {
        unsigned char c = (unsigned char)src[i];

        if (c >= 128 || !isgraph(c)) {
            fprintf(stderr, "Error: custom alphabet contains non-graphical or non-ASCII character.\n");
            return 0;
        }
    }

    return 1;
}
/* Validate that flags starts with '-' and only contains l, u, d, s after that. */
static int validate_flags(const char *flags) {
    if (flags == NULL) {
        return 1;
    }

    if (flags[0] != '-') {
        fprintf(stderr, "Error: flags must begin with '-'.\n");
        return 0;
    }

    if (flags[1] == '\0') {
        fprintf(stderr, "Error: flags cannot be only '-'.\n");
        return 0;
    }
    /* For each character in flags if it's not one of the valid flag characters, return 0. */
    for (size_t i = 1; flags[i] != '\0'; i++) {
        if (flags[i] != 'l' &&
            flags[i] != 'u' &&
            flags[i] != 'd' &&
            flags[i] != 's') {
            fprintf(stderr, "Error: invalid flag '%c'.\n", flags[i]);
            return 0;
        }
    }

    return 1;
}

/* Returns 1 if flags contains the given flag character, else 0. */
static int has_flag(const char *flags, char flag_char) {
    if (flags == NULL) {
        return 0;
    }
    /* For each character in flags, if it's not one of the valid flag characters, return 0. */
    for (size_t i = 1; flags[i] != '\0'; i++) {
        if (flags[i] == flag_char) {
            return 1;
        }
    }
    /* If we didn't find the flag character, return 0. */
    return 0;
}

int build_alphabet(const char *flags,
                   const char *custom_alphabet,
                   char *out_alphabet,
                   size_t out_size) {
    if (out_alphabet == NULL || out_size == 0) {
        fprintf(stderr, "Error: invalid alphabet output buffer.\n");
        return -1;
    }

    if (!validate_flags(flags)) {
        return -1;
    }

    if (!validate_custom_alphabet(custom_alphabet)) {
        return -1;
    }

    int used[128] = {0};
    size_t index = 0;

    /*
     * If the user gives no flags and no custom alphabet,
     * default to all built-in groups. 
     */
    int use_defaults = (flags == NULL && custom_alphabet == NULL);
    if (use_defaults || has_flag(flags, 'l')) {
        if (!append_unique_chars(LOWERCASE, used, out_alphabet, out_size, &index)) {
            return -1;
        }
    }

    if (use_defaults || has_flag(flags, 'u')) {
        if (!append_unique_chars(UPPERCASE, used, out_alphabet, out_size, &index)) {
            return -1;
        }
    }

    if (use_defaults || has_flag(flags, 'd')) {
        if (!append_unique_chars(DIGITS, used, out_alphabet, out_size, &index)) {
            return -1;
        }
    }

    if (use_defaults || has_flag(flags, 's')) {
        if (!append_unique_chars(SYMBOLS, used, out_alphabet, out_size, &index)) {
            return -1;
        }
    }

    if (!append_unique_chars(custom_alphabet, used, out_alphabet, out_size, &index)) {
        return -1;
    }

    out_alphabet[index] = '\0';
    return (int)index;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include "alphabet.h"
#include "pw_generator.h"
#include "information_content.h"
#include "prng.h"
#include "fileio.h"
/*Random Number File*/
#define RANDOM_FILE "random_numbers.dat"

static void print_usage(const char *prog_name) {
    fprintf(stderr,
            "Usage: %s length quantity [-luds] [alphabet]\n"
            "  length   Length of each password (positive integer)\n"
            "  quantity Number of passwords to generate (positive integer)\n"
            "  -luds    Optional flags selecting lowercase, uppercase, digits, symbols\n"
            "  alphabet Optional custom characters to include in the alphabet\n",
            prog_name);
}

static int parse_positive_int(const char *text, const char *name, int *out_value) {
    if (text == NULL || out_value == NULL || text[0] == '\0') {
        fprintf(stderr, "Error: %s must be a positive integer.\n", name);
        return 0;
    }

    errno = 0;
    char *end = NULL;
    long value = strtol(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || value <= 0 || value > INT_MAX) {
        fprintf(stderr, "Error: %s must be a positive integer.\n", name);
        return 0;
    }

    *out_value = (int)value;
    return 1;
}

static int determine_optional_args(int argc,
                                   char *argv[],
                                   const char **flags,
                                   const char **custom_alphabet) {
    *flags = NULL;
    *custom_alphabet = NULL;

    if (argc == 4) {
        if (argv[3][0] == '-') {
            *flags = argv[3];
        } else {
            *custom_alphabet = argv[3];
        }
    } else if (argc == 5) {
        if (argv[3][0] != '-') {
            fprintf(stderr, "Error: invalid argument order. Flags must come before alphabet.\n");
            return 0;
        }
        *flags = argv[3];
        *custom_alphabet = argv[4];
    }

    return 1;
}

static int verify_random_numbers(const unsigned int *saved,
                                 const unsigned int *loaded,
                                 size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (saved[i] != loaded[i]) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 5) {
        print_usage(argv[0]);
        return 1;
    }
   
    int length = 0;
    int quantity = 0;
  
    const char *flags = NULL;
    const char *custom_alphabet = NULL;
    /* Parse length and quantity, ensuring they are positive integers.*/
    if (!parse_positive_int(argv[1], "length", &length) ||
        !parse_positive_int(argv[2], "quantity", &quantity)) {
        print_usage(argv[0]);
        return 1;
    }

    if (!determine_optional_args(argc, argv, &flags, &custom_alphabet)) {
        print_usage(argv[0]);
        return 1;
    }

    char alphabet[MAX_ALPHABET_SIZE] = {0};
    int alphabet_size = build_alphabet(flags,
                                       custom_alphabet,
                                       alphabet,
                                       sizeof(alphabet));
    if (alphabet_size < 0) {
        fprintf(stderr, "Error: failed to build alphabet.\n");
        return 1;
    }

    if (alphabet_size == 0) {
        fprintf(stderr, "Error: final alphabet is empty.\n");
        return 1;
    }

    size_t total_random_numbers = (size_t)length * (size_t)quantity;
    if ((int)(total_random_numbers / (size_t)length) != quantity) {
        fprintf(stderr, "Error: requested output is too large.\n");
        return 1;
    }
    /*Random numbers for password generation and verification*/
    unsigned int *random_numbers = malloc(total_random_numbers * sizeof(unsigned int));
    unsigned int *loaded_numbers = malloc(total_random_numbers * sizeof(unsigned int));
    char *password = malloc((size_t)length + 1);

    if (random_numbers == NULL || loaded_numbers == NULL || password == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free(random_numbers);
        free(loaded_numbers);
        free(password);
        return 1;
    }

    printf("Using alphabet: %s\n\n", alphabet);

    prng_seed((unsigned int)time(NULL));

    size_t random_index = 0;
    double bits = information_content(length, (size_t)alphabet_size);

    for (int i = 0; i < quantity; i++) {
        if (!generate_password(password,
                               (size_t)length + 1,
                               alphabet,
                               (size_t)alphabet_size,
                               length,
                               random_numbers,
                               &random_index)) {
            fprintf(stderr, "Error: failed to generate password.\n");
            free(random_numbers);
            free(loaded_numbers);
            free(password);
            return 1;
        }

        printf("Password %d: %s\n", i + 1, password);
        printf("Information content: %.2f bits\n\n", bits);
    }

    save_random_numbers_to_file(RANDOM_FILE, random_numbers, random_index);
    load_random_numbers_from_file(RANDOM_FILE, loaded_numbers, random_index);

    if (verify_random_numbers(random_numbers, loaded_numbers, random_index)) {
        printf("Saved and reloaded %zu random numbers from %s successfully.\n",
               random_index,
               RANDOM_FILE);
    } else {
        fprintf(stderr, "Error: loaded random numbers do not match saved values.\n");
        free(random_numbers);
        free(loaded_numbers);
        free(password);
        return 1;
    }

    free(random_numbers);
    free(loaded_numbers);
    free(password);

    return 0;
}

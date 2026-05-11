#include "fileio.h"

#include <stdio.h>
#include <stdlib.h>

/* Writes an array of random numbers to a binary file. */
void save_random_numbers_to_file(const char *filename,
                                 const unsigned int *numbers,
                                 size_t count) {
    if (filename == NULL || numbers == NULL) {
        fprintf(stderr, "Error: invalid arguments for save_random_numbers_to_file.\n");
        exit(EXIT_FAILURE);
    }
    /* Open the file for writing in binary mode. */
    FILE *file = fopen(filename, "wb");
    /* If fopen fails, print an error message and exit. */
    if (file == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    /* Write the random numbers to the file. */
    size_t written = fwrite(numbers, sizeof(unsigned int), count, file);
    /* If fwrite fails, print an error message and exit. */ 
    if (written != count) {
        fprintf(stderr, "Error: failed to write all random numbers to file.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    /* Close the file. If fclose fails, print an error message and exit. */
    if (fclose(file) != 0) {
        perror("Error closing output file");
        exit(EXIT_FAILURE);
    }
}
/* Loads an array of random numbers from a binary file. */
void load_random_numbers_from_file(const char *filename,
                                   unsigned int *numbers,
                                   size_t count) {
    if (filename == NULL || numbers == NULL) {
        fprintf(stderr, "Error: invalid arguments for load_random_numbers_from_file.\n");
        exit(EXIT_FAILURE);
    }
    /* Open the file for reading in binary mode. */
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }
    /* Read the random numbers from the file. */
    size_t read_count = fread(numbers, sizeof(unsigned int), count, file);
    if (read_count != count) {
        fprintf(stderr, "Error: failed to read all random numbers from file.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    /* Close the file. If fclose fails, print an error message and exit. */ 
    if (fclose(file) != 0) {
        perror("Error closing input file");
        exit(EXIT_FAILURE);
    }
}

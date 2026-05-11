#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>

void save_random_numbers_to_file(const char *filename,
                                 const unsigned int *numbers,
                                 size_t count);

void load_random_numbers_from_file(const char *filename,
                                   unsigned int *numbers,
                                   size_t count);

#endif

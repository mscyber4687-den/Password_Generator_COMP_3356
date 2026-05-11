CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g
LDFLAGS = -lm
TARGET = pwgen
SOURCES = main.c alphabet.c pw_generator.c information_content.c prng.c fileio.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) random_numbers.dat

.PHONY: all clean

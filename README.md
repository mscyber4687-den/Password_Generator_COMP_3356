# Password Generator, PRNG, and Entropy Analysis

## Overview
This project generates passwords using a custom pseudo-random number generator (PRNG), computes the estimated information content of each password, and saves the raw PRNG values used during generation to a binary file named `random_numbers.dat`. The program then reloads the binary file and verifies that the loaded values match the saved values.

## Files
- `main.c` - command-line parsing and program coordination
- `alphabet.c` / `alphabet.h` - alphabet construction, duplicate removal, and character validation
- `pw_generator.c` / `pw_generator.h` - password generation using the PRNG
- `information_content.c` / `information_content.h` - information content calculation
- `prng.c` / `prng.h` - Linear Congruential Generator implementation
- `fileio.c` / `fileio.h` - binary file write/read functions
- `Makefile` - build system

## How to Compile
Run:

```bash
make
```

Build the executable:

```bash
./pwgen
```

Clean generated files:

```bash
make clean
```

## How to Run
Command format:

```bash
./pwgen length quantity [-luds] [alphabet]
```

Arguments:
- `length` - length of each generated password
- `quantity` - number of passwords to generate
- `-luds` - optional character group flags:
  - `l` lowercase letters
  - `u` uppercase letters
  - `d` digits
  - `s` symbols
- `alphabet` - optional custom characters to add to the final alphabet

If no flags and no custom alphabet are provided, the program uses lowercase, uppercase, digits, and symbols by default.

## Test Example Commands
```bash
./pwgen 8 2 -lu
./pwgen 12 3 -ld "_-"
./pwgen 10 5
./pwgen 6 2 abcXYZ123
```

## Test Examples for Output
```text
Using alphabet: abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ

Password 1: KdjAslQp
Information content: 45.60 bits

Password 2: mZqPwErT
Information content: 45.60 bits

Saved and reloaded 16 random numbers from random_numbers.dat successfully.
```

## LCG summary
The PRNG uses a Linear Congruential Generator with the recurrence:

```text
X(n+1) = (1664525 * X(n) + 1013904223) mod 2^32
```

In C, unsigned integer arithmetic wraps around naturally, which provides the modulo `2^32` behavior.

## Modulo Bias Handling
Mapping a random integer directly with `% alphabet_size` can introduce modulo bias when the PRNG range is not evenly divisible by the alphabet size. This program reduces that bias by using rejection sampling: values above the largest usable multiple of `alphabet_size` are rejected and regenerated.

## Security Consideration
An LCG appears random at first because it produces values that change in a pattern that is not obvious from only a few outputs. However, it is still deterministic. If an attacker learns or guesses the seed and constants, the sequence can be predicted.

Predictability is dangerous for password generation because predictable random numbers can lead to predictable passwords. A real password generator should use a cryptographically secure source of randomness, such as `/dev/urandom`, `getrandom()`, `arc4random()`, or a trusted cryptographic library. 

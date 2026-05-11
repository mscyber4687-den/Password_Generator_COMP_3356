#!/usr/bin/env bash
#You may need to run chmod +x test.sh
set -euo pipefail

TEST_NAME="${1:-}"

pass() {
  echo "PASS"
  exit 0
}

fail() {
  echo "FAIL: $1"
  exit 1
}

build_project() {
  make clean >/dev/null 2>&1 || true
  make >/tmp/build_stdout.txt 2>/tmp/build_stderr.txt || {
    cat /tmp/build_stdout.txt || true
    cat /tmp/build_stderr.txt || true
    fail "make failed"
  }

  [[ -x ./pwgen ]] || fail "expected executable ./pwgen not found"
}

run_and_capture() {
  local outfile="$1"
  shift
  "$@" >"$outfile" 2>&1
}

count_password_lines() {
  local file="$1"
  grep -Eic 'password[[:space:]]*[0-9]*[[:space:]]*:|^password:' "$file" || true
}

extract_password_strings() {
  local file="$1"
  grep -Ei '^password[[:space:]]*([0-9]+)?[[:space:]]*:' "$file" | sed -E 's/^.*:[[:space:]]*//'
}

contains_info_line() {
  local file="$1"
  grep -Eiq 'information content' "$file"
}

contains_alphabet_line() {
  local file="$1"
  grep -Eiq 'using alphabet' "$file"
}

case "$TEST_NAME" in
  build)
    build_project
    pass
    ;;

  basic_cli)
    build_project
    run_and_capture /tmp/out.txt ./pwgen 8 2 -lu

    grep -qi "segmentation fault" /tmp/out.txt && fail "program crashed"
    contains_alphabet_line /tmp/out.txt || fail "missing alphabet output"
    contains_info_line /tmp/out.txt || fail "missing information content output"

    pw_count=$(count_password_lines /tmp/out.txt)
    [[ "$pw_count" -ge 2 ]] || fail "expected at least 2 password lines"

    pass
    ;;

  default_alphabet)
    build_project
    run_and_capture /tmp/out.txt ./pwgen 6 1

    contains_alphabet_line /tmp/out.txt || fail "missing alphabet line"

    grep -Eq '[a-z]' /tmp/out.txt || fail "default alphabet/output seems to lack lowercase"
    grep -Eq '[A-Z]' /tmp/out.txt || fail "default alphabet/output seems to lack uppercase"
    grep -Eq '[0-9]' /tmp/out.txt || fail "default alphabet/output seems to lack digits"

    pass
    ;;

  flags_lu)
    build_project
    run_and_capture /tmp/out.txt ./pwgen 10 3 -lu

    contains_alphabet_line /tmp/out.txt || fail "missing alphabet line"
    grep -Eq '[a-z]' /tmp/out.txt || fail "missing lowercase handling"
    grep -Eq '[A-Z]' /tmp/out.txt || fail "missing uppercase handling"

    passwords=$(extract_password_strings /tmp/out.txt || true)
    [[ -n "$passwords" ]] || fail "could not find generated passwords"

    while IFS= read -r pw; do
      [[ -z "$pw" ]] && continue
      echo "$pw" | grep -Eq '^[A-Za-z]+$' || fail "password contains characters outside -lu set: $pw"
    done <<< "$passwords"

    pass
    ;;

  flags_ds)
    build_project
    run_and_capture /tmp/out.txt ./pwgen 10 3 -ds

    passwords=$(extract_password_strings /tmp/out.txt || true)
    [[ -n "$passwords" ]] || fail "could not find generated passwords"

    while IFS= read -r pw; do
      [[ -z "$pw" ]] && continue
      echo "$pw" | grep -Eq '^[0-9[:punct:]]+$' || fail "password contains characters outside -ds set: $pw"
    done <<< "$passwords"

    pass
    ;;

  custom_alphabet)
    build_project
    run_and_capture /tmp/out.txt ./pwgen 12 4 -l "_-"

    contains_alphabet_line /tmp/out.txt || fail "missing alphabet line"
    grep -q "_" /tmp/out.txt || fail "custom alphabet character '_' not reflected"
    grep -q "-" /tmp/out.txt || fail "custom alphabet character '-' not reflected"

    passwords=$(extract_password_strings /tmp/out.txt || true)
    [[ -n "$passwords" ]] || fail "could not find generated passwords"

    while IFS= read -r pw; do
      [[ -z "$pw" ]] && continue
      echo "$pw" | grep -Eq '^[a-z_-]+$' || fail "password contains characters outside expected union: $pw"
    done <<< "$passwords"

    pass
    ;;

  pw_count_length)
    build_project
    run_and_capture /tmp/out.txt ./pwgen 8 5 -lu

    passwords=$(extract_password_strings /tmp/out.txt || true)
    [[ -n "$passwords" ]] || fail "no password lines found"

    actual_count=0
    while IFS= read -r pw; do
      [[ -z "$pw" ]] && continue
      actual_count=$((actual_count + 1))
      [[ "${#pw}" -eq 8 ]] || fail "password has incorrect length: $pw"
    done <<< "$passwords"

    [[ "$actual_count" -eq 5 ]] || fail "expected 5 passwords, found $actual_count"

    pass
    ;;

  info_content)
    build_project
    run_and_capture /tmp/out.txt ./pwgen 8 2 -lu

    contains_info_line /tmp/out.txt || fail "missing information content lines"

    # For -lu, alphabet size is typically 52, so info should be near 8 * log2(52) ≈ 45.6 bits.
    # We allow a broad range since formatting varies.
    grep -Eiq '[0-9]+\.[0-9]+|[0-9]+ bits' /tmp/out.txt || fail "information content numeric value not detected"

    pass
    ;;

  file_created)
    build_project
    rm -f random_numbers.dat
    run_and_capture /tmp/out.txt ./pwgen 8 2 -lu

    [[ -f random_numbers.dat ]] || fail "random_numbers.dat was not created"

    pass
    ;;

  file_nonempty)
    build_project
    rm -f random_numbers.dat
    run_and_capture /tmp/out.txt ./pwgen 8 2 -lu

    [[ -f random_numbers.dat ]] || fail "random_numbers.dat missing"
    [[ -s random_numbers.dat ]] || fail "random_numbers.dat is empty"

    pass
    ;;

  invalid_length)
    build_project
    set +e
    ./pwgen 0 2 -lu >/tmp/out.txt 2>&1
    status=$?
    set -e

    [[ "$status" -ne 0 ]] || fail "expected non-zero exit for invalid length"
    grep -Eiq 'error|invalid|length' /tmp/out.txt || fail "missing useful error for invalid length"

    pass
    ;;

  invalid_quantity)
    build_project
    set +e
    ./pwgen 8 0 -lu >/tmp/out.txt 2>&1
    status=$?
    set -e

    [[ "$status" -ne 0 ]] || fail "expected non-zero exit for invalid quantity"
    grep -Eiq 'error|invalid|quantity' /tmp/out.txt || fail "missing useful error for invalid quantity"

    pass
    ;;

  invalid_flags)
    build_project
    set +e
    ./pwgen 8 2 -x >/tmp/out.txt 2>&1
    status=$?
    set -e

    [[ "$status" -ne 0 ]] || fail "expected non-zero exit for invalid flags"
    grep -Eiq 'error|invalid|flag' /tmp/out.txt || fail "missing useful error for invalid flags"

    pass
    ;;

  clean_target)
    build_project
    make clean >/tmp/clean_out.txt 2>/tmp/clean_err.txt || fail "make clean failed"

    [[ ! -f ./pwgen ]] || fail "make clean did not remove executable"

    pass
    ;;

  required_files)
    required=(
      "main.c"
      "alphabet.c"
      "alphabet.h"
      "pw_generator.c"
      "pw_generator.h"
      "information_content.c"
      "information_content.h"
      "prng.c"
      "prng.h"
      "fileio.c"
      "fileio.h"
      "Makefile"
      "README.md"
    )

    missing=0
    for f in "${required[@]}"; do
      if [[ ! -f "$f" ]]; then
        echo "Missing: $f"
        missing=1
      fi
    done

    [[ "$missing" -eq 0 ]] || fail "one or more required files missing"

    pass
    ;;

  *)
    fail "unknown test name: $TEST_NAME"
    ;;
esac

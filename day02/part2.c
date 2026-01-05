#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <aocc_common.h>

static bool is_already_periodic(long long n) {
  char buffer[32];
  int len = sprintf(buffer, "%lld", n);
  for (int p = 1; p <= len / 2; p++) {
    if (len % p == 0) {
      bool match = true;
      for (int i = p; i < len; i++) {
        if (buffer[i] != buffer[i % p]) {
          match = false;
          break;
        }
      }
      if (match) {
        return true;
      }
    }
  }
  return false;
}

void generate_invalid_ids(long long start, long long end, long long *total) {
  long long powers_of_10[] = {1,      10,      100,      1000,      10000,
                              100000, 1000000, 10000000, 100000000, 1000000000};

  for (int pattern_len = 1; pattern_len <= 9; pattern_len++) {
    long long pattern_mult = powers_of_10[pattern_len];

    long long pattern_start = powers_of_10[pattern_len - 1];
    long long pattern_end = powers_of_10[pattern_len] - 1;

    if (pattern_start * (pattern_mult + 1) > end)
      break;

    for (long long pattern = pattern_start; pattern <= pattern_end; pattern++) {
      // Skip patterns that are themselves repetitions of a smaller pattern
      if (is_already_periodic(pattern))
        continue;

      long long invalid_id = pattern;
      long long position_mult = 1;

      for (int k = 2; k <= 18; k++) {
        position_mult *= pattern_mult;
        invalid_id += pattern * position_mult;

        if (invalid_id > end)
          break;
        if (invalid_id >= start)
          *total += invalid_id;
      }

      if (pattern * (pattern_mult + 1) > end)
        break;
    }
  }
}

long long sum_invalid_ids(char *input) {
  long long total = 0;
  const char *p = input;

  while (*p != '\0') {
    p += aocc_scan_to_digit(p);
    if (*p == '\0')
      break;

    long long start = aocc_str_of_long(&p);

    p += aocc_scan_to_digit(p);
    if (*p == '\0')
      break;
    long long end = aocc_str_of_long(&p);

    generate_invalid_ids(start, end, &total);
  }

  return total;
}

int main(int argc, char **argv) {
  char *input;
  if (aocc_read_input_file(argc, argv, &input) != 0) {
    return 1;
  }
  printf("%lld\n", sum_invalid_ids(input));

  free(input);

  return 0;
}

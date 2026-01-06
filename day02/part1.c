#include <stdio.h>
#include <stdlib.h>

#include <aocc_common.h>

/**
 * Invalid ID = X . 10^n + X = X * (10^n + 1)
 */
void generate_invalid_ids(long long start, long long end, long long *total) {
  long long powers_of_10[] = {1,      10,      100,      1000,      10000,
                              100000, 1000000, 10000000, 100000000, 1000000000};

  for (int n = 1; n <= 9; n++) {
    long long multiplier = powers_of_10[n] + 1;

    long long half_start = powers_of_10[n - 1];
    long long half_end = powers_of_10[n] - 1;

    for (long long X = half_start; X <= half_end; X++) {
      long long invalid_id = X * multiplier;
      if (invalid_id > end)
        break;
      if (invalid_id >= start)
        *total += invalid_id;
    }
  }
}

static bool is_already_periodic(long long n) {
  char s[32];
  int len = sprintf(s, "%lld", n);
  for (int p = 1; p <= len / 2; p++) {
    if (len % p != 0)
      continue;
    bool periodic = true;
    for (int i = 0; i < len - p; i++) {
      if (s[i] != s[i + p]) {
        periodic = false;
        break;
      }
    }
    if (periodic)
      return true;
  }
  return false;
}

long long sum_invalid_ids(char *input) {
  long long total = 0;
  const char *p = input;

  while (*p != '\0') {
    p += aocc_scan_to_digit(p);
    if (*p == '\0')
      break;

    long long start = aocc_str_to_long(&p);

    p += aocc_scan_to_digit(p);
    if (*p == '\0')
      break;

    long long end = aocc_str_to_long(&p);

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

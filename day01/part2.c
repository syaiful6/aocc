#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aocc_common.h>

long long static aocc_rotate(long long *current, const char *line) {
  if (!line || line[0] == '\0')
    return 0;

  char direction = line[0];
  long long distance = strtol(line + 1, NULL, 10);
  long long zero_crossed = 0;
  int tick = (direction == 'R') ? 1 : -1;

  for (long long i = 0; i < distance; i++) {
    *current = (*current + tick + 100) % 100;
    if (*current == 0) {
      zero_crossed++;
    }
  }

  return zero_crossed;
}

long long aocc_count_zero_rotations(char *input) {
  long long cnt = 0;
  long long cur_pos = 50;
  char *line = strtok(input, "\n");

  while (line != NULL) {
    cnt += aocc_rotate(&cur_pos, line);
    line = strtok(NULL, "\n");
  }

  return cnt;
}

int main(int argc, char **argv) {
  char *input;
  if (aocc_read_input_file(argc, argv, &input) != 0) {
    return 1;
  }
  printf("%lld\n", aocc_count_zero_rotations(input));

  free(input);

  return 0;
}

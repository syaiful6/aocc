#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aocc_common.h>

static int aocc_rotate(long long *current, const char *line) {
  if (!line || line[0] == '\0')
    return 0;

  char direction = line[0];
  long long distance = strtol(line + 1, NULL, 10);
  int delta = (direction == 'R' ? 1 : -1) * (distance % 100);

  *current = (*current + delta + 100) % 100;

  return (*current == 0);
}

long long aocc_final_position(char *input) {
  long long cur_pos = 50;
  long long total = 0;
  char *line = strtok(input, "\n");

  while (line != NULL) {
    total += aocc_rotate(&cur_pos, line);
    line = strtok(NULL, "\n");
  }

  return total;
}

int main(int argc, char **argv) {
  char *input;
  if (aocc_read_input_file(argc, argv, &input) != 0) {
    return 1;
  }
  printf("%lld\n", aocc_final_position(input));

  free(input);

  return 0;
}

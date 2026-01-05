#ifndef AOCC_COMMON_H
#define AOCC_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Read the entire content of a file into a dynamically allocated string.
 * @param filename The path to the file to read.
 * @param content Pointer to a char pointer where the content will be stored.
 *                Caller is responsible for freeing the allocated memory.
 * @return 0 on success, -1 on failure.
 */
static inline int aocc_read_file(const char *filename, char **content) {
  if (!filename || !content)
    return -1;

  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("fopen");
    return -1;
  }

  if (fseek(f, 0, SEEK_END) != 0) {
    perror("fseek");
    fclose(f);
    return -1;
  }

  long len = ftell(f);
  if (len < 0) {
    perror("ftell");
    fclose(f);
    return -1;
  }

  if (fseek(f, 0, SEEK_SET) != 0) {
    perror("fseek");
    fclose(f);
    return -1;
  }

  char *buffer = malloc(len + 1);
  if (!buffer) {
    perror("malloc");
    fclose(f);
    return -1;
  }

  size_t read_size = fread(buffer, 1, len, f);
  if (ferror(f)) {
    perror("fread");
    free(buffer);
    fclose(f);
    return -1;
  }

  buffer[read_size] = '\0';
  fclose(f);
  *content = buffer;
  return 0;
}

/**
 * Read input file from command line arguments.
 * @param argc Argument count from main.
 * @param argv Argument vector from main.
 * @param content Pointer to a char pointer where the content will be stored.
 *                Caller is responsible for freeing the allocated memory.
 * @return 0 on success, -1 on failure.
 */
static inline int aocc_read_input_file(int argc, char **argv, char **content) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filepath>\n", argv[0]);
    return -1;
  }

  return aocc_read_file(argv[1], content);
}

#endif // aocc_COMMON_H

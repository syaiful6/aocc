#include "aocc_common.h"
#include <stddef.h>
#include <stdint.h>
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
inline int aocc_read_file(const char *filename, char **content) {
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
int aocc_read_input_file(int argc, char **argv, char **content) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filepath>\n", argv[0]);
    return -1;
  }

  return aocc_read_file(argv[1], content);
}

size_t aocc_scan_to_delimeter(const char *content, char delim) {
#if defined(PLATFORM_X86)
  __m256i target = _mm256_set1_epi8(delim);
  __m256i null_vec = _mm256_set1_epi8('\0');
  __m256i data = _mm256_loadu_si256((const __m256i *)content);

  // Check for both delimiter and null terminator
  __m256i cmp_delim = _mm256_cmpeq_epi8(data, target);
  __m256i cmp_null = _mm256_cmpeq_epi8(data, null_vec);
  __m256i cmp = _mm256_or_si256(cmp_delim, cmp_null);

  uint32_t mask = (uint32_t)_mm256_movemask_epi8(cmp);

  return (mask != 0) ? (size_t)__builtin_ctz(mask) : 32;
#elif defined(PLATFORM_ARM)
  uint8x16_t target = vmovq_n_u8(delim);
  uint8x16_t null_vec = vmovq_n_u8('\0');
  uint8x16_t data = vld1q_u8((const uint8_t *)content);

  // Check for both delimiter and null terminator
  uint8x16_t cmp_delim = vceqq_u8(data, target);
  uint8x16_t cmp_null = vceqq_u8(data, null_vec);
  uint8x16_t cmp = vorrq_u8(cmp_delim, cmp_null);

  uint8x8_t narrowed = vshrn_n_u16(vreinterpretq_u16_u8(cmp), 4);
  uint64_t mask = vget_lane_u64(vreinterpret_u64_u8(narrowed), 0);

  return (mask != 0) ? (size_t)__builtin_ctzll(mask) : 16;
#else
  // Fallback to scalar implementation
  size_t i = 0;
  while (content[i] != delim && content[i] != '\0')
    i++;
  return i;
#endif
}

size_t aocc_scan_to_non_digit(const char *content) {
#if defined(PLATFORM_X86)
  __m256i data = _mm256_loadu_si256((const __m256i *)content);

  // Check if character is a digit ('0' <= c <= '9')
  __m256i gt_0 = _mm256_cmpgt_epi8(data, _mm256_set1_epi8('0' - 1));
  __m256i lt_9 = _mm256_cmpgt_epi8(_mm256_set1_epi8('9' + 1), data);
  __m256i is_digit_vec = _mm256_and_si256(gt_0, lt_9);

  // Also check for null terminator
  __m256i is_null = _mm256_cmpeq_epi8(data, _mm256_set1_epi8('\0'));

  // Find non-digits or null
  __m256i is_non_digit_or_null =
      _mm256_andnot_si256(is_digit_vec, _mm256_set1_epi8(0xFF));
  is_non_digit_or_null = _mm256_or_si256(is_non_digit_or_null, is_null);

  uint32_t mask = (uint32_t)_mm256_movemask_epi8(is_non_digit_or_null);
  return (mask != 0) ? (size_t)__builtin_ctz(mask) : 32;
#elif defined(PLATFORM_ARM)
  uint8x16_t data = vld1q_u8((const uint8_t *)content);

  // Check if character is a digit
  uint8x16_t is_digit_vec = vandq_u8(vcgeq_u8(data, vmovq_n_u8('0')),
                                     vcleq_u8(data, vmovq_n_u8('9')));

  // Check for null terminator
  uint8x16_t is_null = vceqq_u8(data, vmovq_n_u8('\0'));

  // Find non-digits or null (invert is_digit and OR with is_null)
  uint8x16_t is_non_digit_or_null = vornq_u8(is_null, is_digit_vec);

  uint8x8_t narrowed =
      vshrn_n_u16(vreinterpretq_u16_u8(is_non_digit_or_null), 4);
  uint64_t mask = vget_lane_u64(vreinterpret_u64_u8(narrowed), 0);

  return (mask != 0) ? (size_t)__builtin_ctzll(mask) : 16;
#else
  // Fallback to scalar implementation
  size_t i = 0;
  while (content[i] >= '0' && content[i] <= '9' && content[i] != '\0')
    i++;
  return i;
#endif
}

size_t aocc_scan_to_digit(const char *content) {
#if defined(PLATFORM_X86)
  __m256i data = _mm256_loadu_si256((const __m256i *)content);

  // Check if character is a digit ('0' <= c <= '9')
  __m256i gt_0 = _mm256_cmpgt_epi8(data, _mm256_set1_epi8('0' - 1));
  __m256i lt_9 = _mm256_cmpgt_epi8(_mm256_set1_epi8('9' + 1), data);
  __m256i is_digit = _mm256_and_si256(gt_0, lt_9);

  // Also check for null terminator (stop if we hit it)
  __m256i is_null = _mm256_cmpeq_epi8(data, _mm256_set1_epi8('\0'));

  // Find digits or null
  __m256i is_digit_or_null = _mm256_or_si256(is_digit, is_null);

  uint32_t mask = (uint32_t)_mm256_movemask_epi8(is_digit_or_null);

  return (mask != 0) ? (size_t)__builtin_ctz(mask) : 32;
#elif defined(PLATFORM_ARM)
  uint8x16_t data = vld1q_u8((const uint8_t *)content);

  // Check if character is a digit
  uint8x16_t is_digit_vec = vandq_u8(vcgeq_u8(data, vmovq_n_u8('0')),
                                     vcleq_u8(data, vmovq_n_u8('9')));

  // Check for null terminator
  uint8x16_t is_null = vceqq_u8(data, vmovq_n_u8('\0'));

  // Find digits or null
  uint8x16_t is_digit_or_null = vorrq_u8(is_digit_vec, is_null);

  uint8x8_t narrowed = vshrn_n_u16(vreinterpretq_u16_u8(is_digit_or_null), 4);
  uint64_t mask = vget_lane_u64(vreinterpret_u64_u8(narrowed), 0);

  return (mask != 0) ? (size_t)__builtin_ctzll(mask) : 16;
#else
  // Fallback to scalar implementation
  size_t i = 0;
  while (content[i] != '\0' && (content[i] < '0' || content[i] > '9'))
    i++;
  return i;
#endif
}

// Optimize conversion of string to long long
// for positive integers
long long aocc_str_to_long(const char **ptr) {
  const char *p = *ptr;
  long long val = 0;
  while (*p >= '0' && *p <= '9') {
    val = val * 10 + (*p - '0');
    p++;
  }
  *ptr = p;
  return val;
}

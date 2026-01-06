#ifndef AOCC_COMMON_H
#define AOCC_COMMON_H

#include "stdint.h"

#if defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>
#define PLATFORM_ARM
#elif defined(__AVX2__) || defined(__x86_64__)
#include <immintrin.h>
#define PLATFORM_X86
#endif

int aocc_read_file(const char *filename, char **content);
int aocc_read_input_file(int argc, char **argv, char **content);
size_t aocc_scan_to_delimeter(const char *content, char delim);
size_t aocc_scan_to_non_digit(const char *content);
size_t aocc_scan_to_digit(const char *content);
long long aocc_str_to_long(const char **ptr);

#endif

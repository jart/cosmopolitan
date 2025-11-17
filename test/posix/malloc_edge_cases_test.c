// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-size-larger-than="

int main() {
  void *ptr;

  // Test 1: malloc(0) behavior
  // Different implementations return different things
  ptr = malloc(0);
  if (ptr != NULL) {
    free(ptr);  // Should be safe to free
    printf("Test 1 PASS: malloc(0) returned non-NULL, free() succeeded\n");
  } else {
    printf("Test 1 PASS: malloc(0) returned NULL\n");
  }

  // Test 2: Double free detection would crash, so we test safe patterns instead
  ptr = malloc(100);
  if (!ptr)
    return 1;
  free(ptr);
  ptr = NULL;  // Good practice to avoid double-free
  free(ptr);   // free(NULL) should be safe
  printf("Test 2 PASS: free(NULL) handled safely\n");

  // Test 3: Very large allocation handling
  // This should fail gracefully, not crash
  ptr = malloc(SIZE_MAX);
  if (ptr == NULL) {
    printf("Test 3 PASS: Extremely large malloc() failed safely\n");
  } else {
    free(ptr);
    printf("Test 3 WARNING: Extremely large malloc() succeeded\n");
  }

  // Test 4: Alignment requirements
  // malloc should return properly aligned pointers
  ptr = malloc(1);
  if (ptr && ((uintptr_t)ptr % sizeof(void *)) == 0) {
    printf("Test 4 PASS: Small malloc() properly aligned\n");
    free(ptr);
  } else {
    printf("Test 4 FAIL: Alignment issue\n");
    if (ptr)
      free(ptr);
    return 4;
  }

  // Test 5: realloc edge cases
  ptr = malloc(100);
  if (!ptr)
    return 5;

  // realloc with NULL should behave like malloc
  void *ptr2 = realloc(NULL, 50);
  if (ptr2) {
    free(ptr2);
    printf("Test 5a PASS: realloc(NULL, size) works like malloc\n");
  } else {
    printf("Test 5a FAIL: realloc(NULL, size) failed\n");
    free(ptr);
    return 5;
  }

  // realloc with size 0 should behave like free (implementation dependent)
  ptr = realloc(ptr, 0);
  if (ptr == NULL) {
    printf("Test 5b PASS: realloc(ptr, 0) freed memory\n");
  } else {
    printf("Test 5b WARNING: realloc(ptr, 0) returned non-NULL\n");
    free(ptr);
  }

  // Test 6: calloc overflow protection
  // calloc(SIZE_MAX/2, 4) would overflow SIZE_MAX if not protected
  ptr = calloc(SIZE_MAX / 2, 4);
  if (ptr == NULL) {
    printf("Test 6 PASS: calloc overflow protection works\n");
  } else {
    free(ptr);
    printf("Test 6 WARNING: Large calloc succeeded (check for overflow)\n");
  }

  // Test 7: Zero-sized calloc
  ptr = calloc(0, 10);
  if (ptr == NULL) {
    printf("Test 7a PASS: calloc(0, 10) returned NULL\n");
  } else {
    free(ptr);
    printf("Test 7a PASS: calloc(0, 10) returned valid pointer\n");
  }

  ptr = calloc(10, 0);
  if (ptr == NULL) {
    printf("Test 7b PASS: calloc(10, 0) returned NULL\n");
  } else {
    free(ptr);
    printf("Test 7b PASS: calloc(10, 0) returned valid pointer\n");
  }

  printf("All critical malloc tests passed!\n");
  return 0;
}
#pragma GCC diagnostic pop
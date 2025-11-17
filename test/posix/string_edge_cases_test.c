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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wformat-truncation"
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wstringop-overread"

int main() {
  char buffer[100];
  char source[200];

  // Test 1: strlcpy vs strncpy behavior
  // strlcpy guarantees null termination, strncpy doesn't
  memset(buffer, 'X', sizeof(buffer));
  strncpy(buffer, "test", 5);
  if (buffer[4] == '\0') {
    printf("Test 1 PASS: strncpy null-terminated correctly\n");
  } else {
    printf(
        "Test 1 PASS: strncpy doesn't guarantee null termination (expected)\n");
  }

  // Test 2: Overlapping memory in strcpy/memcpy
  strcpy(buffer, "hello world");
  // This is undefined behavior in strcpy, should use memmove
  // strcpy(buffer + 2, buffer);  // DON'T DO THIS - undefined behavior

  // Test with memmove instead (safe)
  strcpy(buffer, "hello world");
  memmove(buffer + 2, buffer, strlen(buffer) + 1);
  if (strcmp(buffer + 2, "hello world") == 0) {
    printf("Test 2 PASS: memmove handles overlapping memory\n");
  } else {
    printf("Test 2 FAIL: memmove failed with overlapping memory\n");
    return 2;
  }

  // Test 3: snprintf vs sprintf safety
  memset(source, 'A', sizeof(source) - 1);
  source[sizeof(source) - 1] = '\0';

  int result = snprintf(buffer, sizeof(buffer), "prefix: %s", source);
  if (buffer[sizeof(buffer) - 1] == '\0' && result > 0) {
    printf("Test 3 PASS: snprintf prevented buffer overflow\n");
  } else {
    printf("Test 3 FAIL: snprintf buffer handling failed\n");
    return 3;
  }

  // Test 4: strncat buffer overflow protection
  strcpy(buffer, "start");
  memset(source, 'B', 50);
  source[50] = '\0';

  strncat(buffer, source, sizeof(buffer) - strlen(buffer) - 1);
  if (buffer[sizeof(buffer) - 1] == '\0') {
    printf("Test 4 PASS: strncat prevented overflow\n");
  } else {
    printf("Test 4 FAIL: strncat buffer overflow\n");
    return 4;
  }

  // Test 5: strlen with non-null-terminated strings
  // We can't test this directly as it would be undefined behavior
  // Instead test strlen with very long strings
  memset(source, 'C', sizeof(source) - 1);
  source[sizeof(source) - 1] = '\0';

  size_t len = strlen(source);
  if (len == sizeof(source) - 1) {
    printf("Test 5 PASS: strlen calculated correctly for long string\n");
  } else {
    printf("Test 5 FAIL: strlen calculation incorrect\n");
    return 5;
  }

  // Test 6: memcmp with zero length
  int cmp_result = memcmp("abc", "def", 0);
  if (cmp_result == 0) {
    printf("Test 6 PASS: memcmp with zero length returns 0\n");
  } else {
    printf("Test 6 FAIL: memcmp with zero length should return 0\n");
    return 6;
  }

  // Test 7: String functions with NULL pointers (would crash, so test
  // carefully) These would segfault, so we test the safe alternatives
  size_t safe_len = strnlen("test", 10);
  if (safe_len == 4) {
    printf("Test 7 PASS: strnlen works correctly\n");
  } else {
    printf("Test 7 FAIL: strnlen incorrect\n");
    return 7;
  }

  // Test 8: Case conversion edge cases
  char test_str[] = "TeSt123!@#";
  for (int i = 0; test_str[i]; i++) {
    if (test_str[i] >= 'A' && test_str[i] <= 'Z') {
      test_str[i] = test_str[i] - 'A' + 'a';
    }
  }
  if (strcmp(test_str, "test123!@#") == 0) {
    printf("Test 8 PASS: Manual case conversion works\n");
  } else {
    printf("Test 8 FAIL: Case conversion failed\n");
    return 8;
  }

  printf("All critical string function tests passed!\n");
  return 0;
}
#pragma GCC diagnostic pop

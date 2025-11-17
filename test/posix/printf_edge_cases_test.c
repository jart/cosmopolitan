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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Walloc-size-larger-than="
#pragma GCC diagnostic ignored "-Wformat-extra-args"
#pragma GCC diagnostic ignored "-Wformat-truncation"
#pragma GCC diagnostic ignored "-Wformat="
#pragma GCC diagnostic ignored "-Wstringop-overread"

int main() {
  char buffer[1024];
  int result;

  // Test 1: %n vulnerability prevention
  // Historically, %n could be used to write to arbitrary memory
  int write_target = 0;
  result = snprintf(buffer, sizeof(buffer), "test%n", &write_target);
  if (result < 0) {
    // Good: %n is disabled/rejected
    printf("Test 1 PASS: %%n properly rejected\n");
  } else if (write_target == 4) {
    // Potentially dangerous: %n worked
    printf("Test 1 WARNING: %%n is enabled (potential security risk)\n");
  } else {
    printf("Test 1 FAIL: Unexpected behavior\n");
    return 1;
  }

  // Test 2: Excessive width specifier DoS protection
  // sprintf(buf, "%*s", INT_MAX, "test") used to cause crashes
  result = snprintf(buffer, sizeof(buffer), "%*s", 0x0fffff00, "test");
  if (result == 0x0fffff00) {
    printf("Test 2 PASS: Excessive width handled safely\n");
  } else {
    printf("Test 2 FAIL: Should have failed or truncated\n");
    return 2;
  }

  // Test 3: Negative width/precision handling
  result = snprintf(buffer, sizeof(buffer), "%*.*s", -10, -5, "test");
  if (result >= 0 && result < sizeof(buffer)) {
    printf("Test 3 PASS: Negative width/precision handled\n");
  } else {
    printf("Test 3 FAIL: Negative parameters caused issues\n");
    return 3;
  }

  // Test 4: NULL string pointer with %s
  result = snprintf(buffer, sizeof(buffer), "%s", (char*)NULL);
  if (result >= 0 && (strstr(buffer, "(null)") || strstr(buffer, "NULL") ||
                      strlen(buffer) == 0)) {
    printf("Test 4 PASS: NULL pointer handled safely\n");
  } else {
    printf("Test 4 FAIL: NULL pointer not handled safely\n");
    return 4;
  }

  // Test 5: Buffer overflow protection in snprintf
  char small_buf[5];
  result =
      snprintf(small_buf, sizeof(small_buf), "this_is_way_too_long_for_buffer");
  if (result > 0 && small_buf[4] == '\0') {
    printf("Test 5 PASS: snprintf properly null-terminated\n");
  } else {
    printf("Test 5 FAIL: Buffer overflow or termination issue\n");
    return 5;
  }

  // Test 6: %% handling edge case
  result = snprintf(buffer, sizeof(buffer), "%%%%");
  if (result == 2 && strcmp(buffer, "%%") == 0) {
    printf("Test 6 PASS: %%%% correctly produces %%%%\n");
  } else {
    printf("Test 6 FAIL: %%%% handling incorrect\n");
    return 6;
  }

  // Test 7: Invalid format specifier handling
  result = snprintf(buffer, sizeof(buffer), "%Q", 42);  // %Q is not standard
  if (result < 0) {
    printf("Test 7 PASS: Invalid format specifier rejected\n");
  } else {
    printf("Test 7 WARNING: Invalid format specifier accepted\n");
  }

  printf("All critical printf tests passed!\n");
  return 0;
}

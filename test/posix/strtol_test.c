/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#define TEST(x) \
  if (!(x))     \
  return __LINE__

int main() {
  char *end;

  // Basic conversions
  TEST(strtol("10", &end, 10) == 10L);
  TEST(strtol("-10", &end, 10) == -10L);
  TEST(strtol("+10", &end, 10) == 10L);

  // Edge cases and error detection
  TEST(strtol("0", &end, 10) == 0L);
  TEST(strtol("-0", &end, 10) == 0L);
  TEST(strtol("+0", &end, 10) == 0L);
  TEST(strtol("9223372036854775807", &end, 10) == LONG_MAX);
  TEST(strtol("-9223372036854775808", &end, 10) == LONG_MIN);

  // Base specification
  TEST(strtol("10", &end, 2) == 2L);
  TEST(strtol("10", &end, 16) == 16L);

  // Invalid input (should not modify errno if conversion is successful)
  errno = 0;
  TEST(strtol("invalid", &end, 10) == 0L && errno == 0);

  // Overflow detection
  errno = 0;
  TEST(strtol("99999999999999999999999999", &end, 10) == LONG_MAX &&
       errno == ERANGE);

  // Underflow detection
  errno = 0;
  TEST(strtol("-99999999999999999999999999", &end, 10) == LONG_MIN &&
       errno == ERANGE);

  // Partial conversion with valid characters before invalid ones
  TEST(strtol("123abc", &end, 10) == 123L && *end == 'a');

  // Testing with leading white space
  TEST(strtol("   123", &end, 10) == 123L);

  // Base 0 auto-detection
  TEST(strtol("0x10", &end, 0) == 16L);
  TEST(strtol("010", &end, 0) == 8L);
  TEST(strtol("10", &end, 0) == 10L);

  // Check if 'end' pointer is set correctly to the next character after the
  // last valid digit
  char *ptr = "1234abcd";
  strtol(ptr, &end, 10);
  TEST(end == ptr + 4);
}

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

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  double d;
  float f;
  char buffer[100];

  // Test 1: NaN handling and detection
  d = 0.0 / 0.0;  // Creates NaN
  if (isnan(d)) {
    printf("Test 1 PASS: NaN detection works\n");
  } else {
    printf("Test 1 FAIL: NaN not detected properly\n");
    return 1;
  }

  // Test 2: Infinity handling
  d = 1.0 / 0.0;  // Creates positive infinity
  if (isinf(d) && d > 0) {
    printf("Test 2a PASS: Positive infinity detected\n");
  } else {
    printf("Test 2a FAIL: Positive infinity not detected\n");
    return 2;
  }

  d = -1.0 / 0.0;  // Creates negative infinity
  if (isinf(d) && d < 0) {
    printf("Test 2b PASS: Negative infinity detected\n");
  } else {
    printf("Test 2b FAIL: Negative infinity not detected\n");
    return 2;
  }

  // Test 3: NaN comparison behavior (NaN != NaN)
  double nan1 = 0.0 / 0.0;
  double nan2 = 0.0 / 0.0;
  if (!(nan1 == nan2) && !(nan1 < nan2) && !(nan1 > nan2)) {
    printf("Test 3 PASS: NaN comparison behavior correct\n");
  } else {
    printf("Test 3 FAIL: NaN comparison behavior incorrect\n");
    return 3;
  }

  // Test 4: printf formatting of special values
  snprintf(buffer, sizeof(buffer), "%g", 0.0 / 0.0);
  if (strstr(buffer, "nan") || strstr(buffer, "NaN") || strstr(buffer, "NAN")) {
    printf("Test 4a PASS: NaN formats correctly in printf\n");
  } else {
    printf("Test 4a WARNING: NaN formatting: '%s'\n", buffer);
  }

  snprintf(buffer, sizeof(buffer), "%g", 1.0 / 0.0);
  if (strstr(buffer, "inf") || strstr(buffer, "Inf") || strstr(buffer, "INF")) {
    printf("Test 4b PASS: Infinity formats correctly in printf\n");
  } else {
    printf("Test 4b WARNING: Infinity formatting: '%s'\n", buffer);
  }

  // Test 5: Denormal numbers
  f = FLT_MIN / 2.0f;  // Creates a denormal
  if (f > 0.0f && f < FLT_MIN) {
    printf("Test 5 PASS: Denormal numbers supported\n");
  } else {
    printf("Test 5 WARNING: Denormal may have flushed to zero\n");
  }

  // Test 6: strtod parsing edge cases
  char *endptr;

  d = strtod("inf", &endptr);
  if (isinf(d)) {
    printf("Test 6a PASS: strtod parses 'inf' correctly\n");
  } else {
    printf("Test 6a FAIL: strtod failed to parse 'inf'\n");
    return 6;
  }

  d = strtod("nan", &endptr);
  if (isnan(d)) {
    printf("Test 6b PASS: strtod parses 'nan' correctly\n");
  } else {
    printf("Test 6b FAIL: strtod failed to parse 'nan'\n");
    return 6;
  }

  d = strtod("1.23e999", &endptr);
  if (isinf(d)) {
    printf("Test 6c PASS: strtod handles overflow to infinity\n");
  } else {
    printf("Test 6c FAIL: strtod overflow handling incorrect\n");
    return 6;
  }

  // Test 7: Zero handling (-0.0 vs +0.0)
  double pos_zero = 0.0;
  double neg_zero = -0.0;
  if (pos_zero == neg_zero && signbit(neg_zero) && !signbit(pos_zero)) {
    printf("Test 7 PASS: Signed zero handling correct\n");
  } else {
    printf("Test 7 FAIL: Signed zero handling incorrect\n");
    return 7;
  }

  // Test 8: Math library function edge cases
  d = sqrt(-1.0);
  if (isnan(d)) {
    printf("Test 8a PASS: sqrt(-1) returns NaN\n");
  } else {
    printf("Test 8a FAIL: sqrt(-1) should return NaN\n");
    return 8;
  }

  d = log(0.0);
  if (isinf(d) && d < 0) {
    printf("Test 8b PASS: log(0) returns -infinity\n");
  } else {
    printf("Test 8b FAIL: log(0) should return -infinity\n");
    return 8;
  }

  d = pow(0.0, 0.0);
  if (d == 1.0) {
    printf("Test 8c PASS: pow(0,0) returns 1\n");
  } else {
    printf("Test 8c WARNING: pow(0,0) = %g (implementation defined)\n", d);
  }

  printf("All critical floating point tests completed!\n");
  return 0;
}
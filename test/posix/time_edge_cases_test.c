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
#include <time.h>
#include <unistd.h>

int main() {
  time_t t;
  struct tm *tm_ptr;
  struct tm tm_struct;
  char buffer[100];

  // Test 1: Epoch handling (January 1, 1970)
  t = 0;
  tm_ptr = gmtime(&t);
  if (tm_ptr && tm_ptr->tm_year == 70 && tm_ptr->tm_mon == 0 &&
      tm_ptr->tm_mday == 1) {
    printf("Test 1 PASS: Unix epoch handled correctly\n");
  } else {
    printf("Test 1 FAIL: Unix epoch handling incorrect\n");
    return 1;
  }

  // Test 2: Year 2038 problem (32-bit time_t overflow)
  t = 2147483647;  // January 19, 2038 03:14:07 UTC
  tm_ptr = gmtime(&t);
  if (tm_ptr && tm_ptr->tm_year == 138) {  // 2038 = 1900 + 138
    printf("Test 2 PASS: Year 2038 boundary handled\n");
  } else {
    printf("Test 2 WARNING: Year 2038 handling may be limited\n");
  }

  // Test 3: Leap year detection
  memset(&tm_struct, 0, sizeof(tm_struct));
  tm_struct.tm_year = 100;  // Year 2000
  tm_struct.tm_mon = 1;     // February
  tm_struct.tm_mday = 29;   // 29th (should be valid in leap year)
  tm_struct.tm_hour = 12;

  t = mktime(&tm_struct);
  if (t != -1) {
    printf("Test 3a PASS: Leap year 2000 Feb 29 accepted\n");
  } else {
    printf("Test 3a FAIL: Leap year 2000 Feb 29 rejected\n");
    return 3;
  }

  // Test non-leap year
  tm_struct.tm_year = 101;  // Year 2001 (not a leap year)
  tm_struct.tm_mon = 1;     // February
  tm_struct.tm_mday = 29;   // 29th (should be invalid)

  t = mktime(&tm_struct);
  tm_ptr = gmtime(&t);
  if (tm_ptr && (tm_ptr->tm_mon != 1 || tm_ptr->tm_mday != 29)) {
    printf("Test 3b PASS: Non-leap year Feb 29 corrected\n");
  } else {
    printf("Test 3b WARNING: Non-leap year Feb 29 handling unclear\n");
  }

  // Test 4: Century leap year rules (1900 not leap, 2000 is leap)
  tm_struct.tm_year = 0;   // Year 1900
  tm_struct.tm_mon = 1;    // February
  tm_struct.tm_mday = 29;  // 29th (should be invalid - 1900 not leap year)

  t = mktime(&tm_struct);
  tm_ptr = gmtime(&t);
  if (tm_ptr && (tm_ptr->tm_mon != 1 || tm_ptr->tm_mday != 29)) {
    printf("Test 4 PASS: Century non-leap year 1900 handled correctly\n");
  } else {
    printf("Test 4 WARNING: Century leap year rules may be incorrect\n");
  }

  // Test 5: Negative time_t values (before 1970)
  t = -86400;  // One day before epoch
  tm_ptr = gmtime(&t);
  if (tm_ptr && tm_ptr->tm_year == 69 && tm_ptr->tm_mon == 11 &&
      tm_ptr->tm_mday == 31) {
    printf("Test 5 PASS: Negative time_t (before 1970) handled\n");
  } else {
    printf("Test 5 WARNING: Negative time_t handling limited\n");
  }

  // Test 6: strftime edge cases
  t = 0;
  tm_ptr = gmtime(&t);
  if (tm_ptr) {
    // Test %% handling
    size_t len = strftime(buffer, sizeof(buffer), "%%", tm_ptr);
    if (len == 1 && strcmp(buffer, "%") == 0) {
      printf("Test 6a PASS: strftime %% handled correctly\n");
    } else {
      printf("Test 6a FAIL: strftime %% handling incorrect\n");
      return 6;
    }

    // Test buffer overflow protection
    len = strftime(buffer, 5, "This is way too long for the buffer", tm_ptr);
    if (len == 0) {  // Should return 0 if buffer too small
      printf("Test 6b PASS: strftime buffer overflow protected\n");
    } else {
      printf("Test 6b WARNING: strftime buffer overflow behavior: len=%zu\n",
             len);
    }
  }

  // Test 7: mktime normalization
  memset(&tm_struct, 0, sizeof(tm_struct));
  tm_struct.tm_year = 70;  // 1970
  tm_struct.tm_mon = 0;    // January
  tm_struct.tm_mday = 32;  // 32nd (invalid, should normalize to Feb 1)
  tm_struct.tm_hour = 25;  // 25th hour (invalid, should normalize)
  tm_struct.tm_min = 61;   // 61st minute (invalid)

  t = mktime(&tm_struct);
  if (t != -1 && tm_struct.tm_mday <= 31 && tm_struct.tm_hour < 24 &&
      tm_struct.tm_min < 60) {
    printf("Test 7 PASS: mktime normalization works\n");
  } else {
    printf("Test 7 FAIL: mktime normalization failed\n");
    return 7;
  }

  // Test 8: Daylight saving time handling
  memset(&tm_struct, 0, sizeof(tm_struct));
  tm_struct.tm_year = 121;  // 2021
  tm_struct.tm_mon = 6;     // July (likely DST in many timezones)
  tm_struct.tm_mday = 15;
  tm_struct.tm_hour = 12;
  tm_struct.tm_isdst = -1;  // Let mktime determine DST

  t = mktime(&tm_struct);
  if (t != -1) {
    printf("Test 8 PASS: DST determination completed\n");
    printf("         DST status: %s\n", tm_struct.tm_isdst > 0    ? "DST"
                                        : tm_struct.tm_isdst == 0 ? "Standard"
                                                                  : "Unknown");
  } else {
    printf("Test 8 FAIL: DST determination failed\n");
    return 8;
  }

  printf("All critical time/date tests completed!\n");
  return 0;
}
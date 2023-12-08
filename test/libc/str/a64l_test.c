/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(l64a, test) {
  EXPECT_STREQ("", l64a(0));
  EXPECT_STREQ("zzzzz/", l64a(0x7fffffff));
}

TEST(a64l, test) {
  EXPECT_EQ(0, a64l(""));
  EXPECT_EQ(0x7fffffff, a64l("zzzzz/"));
}

dontinline long openbsd_a64l(const char *s) {
  long value, digit, shift;
  int i;
  if (s == NULL) {
    errno = EINVAL;
    return (-1L);
  }
  value = 0;
  shift = 0;
  for (i = 0; *s && i < 6; i++, s++) {
    if (*s >= '.' && *s <= '/')
      digit = *s - '.';
    else if (*s >= '0' && *s <= '9')
      digit = *s - '0' + 2;
    else if (*s >= 'A' && *s <= 'Z')
      digit = *s - 'A' + 12;
    else if (*s >= 'a' && *s <= 'z')
      digit = *s - 'a' + 38;
    else {
      errno = EINVAL;
      return (-1L);
    }
    value |= digit << shift;
    shift += 6;
  }
  return (value);
}

dontinline char *openbsd_l64a(long value) {
  static char buf[8];
  char *s = buf;
  int digit;
  int i;
  if (value < 0) {
    errno = EINVAL;
    return (NULL);
  }
  for (i = 0; value != 0 && i < 6; i++) {
    digit = value & 0x3f;
    if (digit < 2)
      *s = digit + '.';
    else if (digit < 12)
      *s = digit + '0' - 2;
    else if (digit < 38)
      *s = digit + 'A' - 12;
    else
      *s = digit + 'a' - 38;
    value >>= 6;
    s++;
  }
  *s = '\0';
  return (buf);
}

TEST(openbsd, consistency) {
  long i;
  for (i = 0; i < 512; ++i) {
    ASSERT_STREQ(openbsd_l64a(i), l64a(i));
    ASSERT_EQ(openbsd_a64l(openbsd_l64a(i)), a64l(l64a(i)));
  }
  for (i = INT_MAX; i > INT_MAX - 100; --i) {
    ASSERT_STREQ(openbsd_l64a(i), l64a(i));
    ASSERT_EQ(openbsd_a64l(openbsd_l64a(i)), a64l(l64a(i)));
  }
}

BENCH(a64l, bench) {
  EZBENCH2("l64a", donothing, l64a(INT_MAX));
  EZBENCH2(
      "a64l", donothing,
      a64l("./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));
  EZBENCH2("openbsd_l64a", donothing, openbsd_l64a(INT_MAX));
  EZBENCH2(
      "openbsd_a64l", donothing,
      openbsd_a64l(
          "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));
}

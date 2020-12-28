/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/str.h"
#include "net/http/http.h"

/**
 * Parses HTTP Range request header.
 */
bool ParseHttpRange(const char *p, size_t n, long resourcelength,
                    long *out_start, long *out_length) {
  long start, length, ending;
  *out_start = 0;
  *out_length = 0;
  if (memchr(p, ',', n)) return false;
  if (n < 7 || memcmp(p, "bytes=", 6) != 0) return false;
  p += 6, n -= 6;
  if (n && *p == '-') {
    ++p, --n;
    length = 0;
    while (n && '0' <= *p && *p <= '9') {
      if (__builtin_mul_overflow(length, 10, &length)) return false;
      if (__builtin_add_overflow(length, *p - '0', &length)) return false;
      ++p, --n;
    }
    if (__builtin_sub_overflow(resourcelength, length, &start)) return false;
  } else {
    start = 0;
    while (n && '0' <= *p && *p <= '9') {
      if (__builtin_mul_overflow(start, 10, &start)) return false;
      if (__builtin_add_overflow(start, *p - '0', &start)) return false;
      ++p, --n;
    }
    if (n && *p == '-') {
      ++p, --n;
      length = 0;
      while (n && '0' <= *p && *p <= '9') {
        if (__builtin_mul_overflow(length, 10, &length)) return false;
        if (__builtin_add_overflow(length, *p - '0', &length)) return false;
        ++p, --n;
      }
      if (__builtin_add_overflow(length, 1, &length)) return false;
      if (__builtin_sub_overflow(length, start, &length)) return false;
    } else if (__builtin_sub_overflow(resourcelength, start, &length)) {
      return false;
    }
  }
  if (n) return false;
  if (start < 0) return false;
  if (length < 0) return false;
  *out_start = start;
  *out_length = length;
  if (__builtin_add_overflow(start, length, &ending)) return false;
  if (ending > resourcelength) return false;
  return true;
}

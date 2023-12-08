/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/macros.internal.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "net/http/http.h"

/**
 * Parses HTTP Range request header.
 *
 * Here are some example values:
 *
 *     Range: bytes=0-                 (everything)
 *     Range: bytes=0-499              (first 500 bytes)
 *     Range: bytes=500-999            (second 500 bytes)
 *     Range: bytes=-500               (final 500 bytes)
 *     Range: bytes=0-0,-1             (first and last and always)
 *     Range: bytes=500-600,601-999    (overlong but legal)
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
      if (ckd_mul(&length, length, 10)) return false;
      if (ckd_add(&length, length, *p - '0')) return false;
      ++p, --n;
    }
    if (ckd_sub(&start, resourcelength, length)) return false;
  } else {
    start = 0;
    while (n && '0' <= *p && *p <= '9') {
      if (ckd_mul(&start, start, 10)) return false;
      if (ckd_add(&start, start, *p - '0')) return false;
      ++p, --n;
    }
    if (n && *p == '-') {
      ++p, --n;
      if (!n) {
        length = MAX(start, resourcelength) - start;
      } else {
        length = 0;
        while (n && '0' <= *p && *p <= '9') {
          if (ckd_mul(&length, length, 10)) return false;
          if (ckd_add(&length, length, *p - '0')) return false;
          ++p, --n;
        }
        if (ckd_add(&length, length, 1)) return false;
        if (ckd_sub(&length, length, start)) return false;
      }
    } else if (ckd_sub(&length, resourcelength, start)) {
      return false;
    }
  }
  if (n) return false;
  if (start < 0) return false;
  if (length < 1) return false;
  if (start > resourcelength) return false;
  if (ckd_add(&ending, start, length)) return false;
  if (ending > resourcelength) {
    length = resourcelength - start;
  }
  *out_start = start;
  *out_length = length;
  return true;
}

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
#include "libc/runtime/zipos.internal.h"

// normalizes zip filesystem path w/ overlapping strlcpy() style api
// zip paths look like relative paths, but they're actually absolute
// with respect to the archive; so similar to how /../etc would mean
// /etc, we'd translate that here to "etc", and "etc/" slash is kept
// returns strlen of 𝑑; returns 𝑛 when insufficient buffer available
// nul terminator is guaranteed if n>0. it's fine if 𝑑 and 𝑠 overlap
// test vectors for this algorithm in: test/libc/stdio/zipdir_test.c
size_t __zipos_normpath(char *d, const char *s, size_t n) {
  char *p, *e;
  for (p = d, e = d + n; p < e && *s; ++s) {
    if ((p == d || p[-1] == '/') && *s == '/') {
      // matched "^/" or "//"
    } else if ((p == d || p[-1] == '/') &&  //
               s[0] == '.' &&               //
               (!s[1] || s[1] == '/')) {
      // matched "/./" or "^.$" or "^./" or "/.$"
    } else if ((p == d || p[-1] == '/') &&  //
               s[0] == '.' &&               //
               s[1] == '.' &&               //
               (!s[2] || s[2] == '/')) {
      // matched "/../" or "^..$" or "^../" or "/..$"
      while (p > d && p[-1] == '/') --p;
      while (p > d && p[-1] != '/') --p;
    } else {
      *p++ = *s;
    }
  }
  // if we didn't overflow
  if (p < e) {
    *p = '\0';
  } else {
    // force nul-terminator to exist if possible
    if (p > d) {
      p[-1] = '\0';
    }
  }
  return p - d;
}

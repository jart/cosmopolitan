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
#include "libc/atomic.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/str/str.h"

#define STRS  4
#define BYTES 128

static atomic_uint bufi;
static char bufs[STRS][BYTES];

char *testlib_formatint(intptr_t x) {
  int i = atomic_fetch_add(&bufi, 1) % STRS;
  char *p = bufs[i];
  p = FormatInt64(p, x);
  p = stpcpy(p, " (or ");
  p = FormatHex64(p, x, 1);
  if (0 <= x && x <= 255) {
    p = stpcpy(p, " or '");
    if (!isascii(x) || iscntrl(x)) {
      *p++ = '\\';
      if (x > 7) {
        if (x > 070) {
          *p++ = '0' + ((x & 0700) >> 6);
        }
        *p++ = '0' + ((x & 070) >> 3);
      }
      *p++ = '0' + (x & 7);
    } else if (x == '\\' || x == '\'') {
      *p++ = '\\';
      *p++ = x;
    } else {
      *p++ = x;
    }
    *p++ = '\'';
  }
  if (_strerrno(x)) {
    p = stpcpy(p, " or ");
    p = stpcpy(p, _strerrno(x));
  }
  *p++ = ')';
  *p++ = '\0';
  return bufs[i];
}

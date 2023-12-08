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
#include "libc/intrin/atomic.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#include "libc/testlib/testlib.h"

#define STRS  4
#define BYTES 64

#define APPEND(c)        \
  do {                   \
    if (j + 1 < BYTES) { \
      bufs[i][j++] = c;  \
    }                    \
  } while (0)

static atomic_uint bufi;
static char bufs[STRS][BYTES];

static int AppendWide(wint_t x, int i, int j) {
  uint64_t w;
  if (isascii(x) && iscntrl(x)) {
    APPEND('\\');
    APPEND('0' + ((x & 0700) >> 6));
    APPEND('0' + ((x & 070) >> 3));
    APPEND('0' + (x & 7));
  } else if (x == '\\' || x == '\'') {
    APPEND('\\');
    APPEND(x);
  } else {
    w = tpenc(x);
    while (w) {
      APPEND(w);
      w >>= 8;
    }
  }
  return j;
}

/**
 * Turns string into code.
 */
char *testlib_formatstr(size_t cw, const void *p, int n) {
  int i, j = 0;
  if (!p) return "NULL";
  i = atomic_fetch_add(&bufi, 1) % STRS;
  switch (cw) {
    case 1: {
      const char *s = p;
      if (n < 0) n = s ? strlen(s) : 0;
      const char *se = s + n;
      APPEND('"');
      while (s < se) {
        j = AppendWide(*s++ & 255, i, j);
      }
      break;
    }
    case 2: {
      const char16_t *s = p;
      if (n < 0) n = s ? strlen16(s) : 0;
      const char16_t *se = s + n;
      APPEND('u');
      APPEND('"');
      while (s < se) {
        wint_t x = *s++ & 0xffff;
        if (IsUtf16Cont(x)) continue;
        if (!IsUcs2(x) && s < se) {
          wint_t y = *s++ & 0xffff;
          x = MergeUtf16(x, y);
        }
        j = AppendWide(x, i, j);
      }
      break;
    }
    case 4: {
      const wchar_t *s = p;
      if (n < 0) n = s ? wcslen(s) : 0;
      const wchar_t *se = s + n;
      APPEND('L');
      APPEND('"');
      while (s < se) {
        j = AppendWide(*s++, i, j);
      }
      break;
    }
    default:
      notpossible;
  }
  APPEND('"');
  bufs[i][j] = 0;
  if (j == BYTES - 1) {
    WRITE32LE(bufs[i] + j - 4, READ32LE("...\""));
  }
  return bufs[i];
}

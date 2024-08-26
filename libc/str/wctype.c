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
#include "libc/wctype.h"
#include "libc/macros.h"
#include "libc/serialize.h"

static const char kWcTypeNames[][8] = {
    "alnum",   //
    "alpha",   //
    "blank",   //
    "cntrl",   //
    "digit",   //
    "graph",   //
    "lower",   //
    "print",   //
    "punct",   //
    "space",   //
    "upper",   //
    "xdigit",  //
};

/**
 * Returns number representing character class name.
 *
 * @param s can be "alnum", "alpha", "blank", "cntrl", "digit", "graph",
 *     "lower", "print", "punct", "space", "upper", "xdigit"
 * @return nonzero id or 0 if not found
 */
wctype_t wctype(const char *s) {
  int i;
  char b[8];
  for (i = 0; i < 8; ++i) {
    b[i] = *s ? *s++ : 0;
  }
  if (!*s) {
    for (i = 0; i < ARRAYLEN(kWcTypeNames); ++i) {
      if (READ64LE(b) == READ64LE(kWcTypeNames[i])) {
        return i + 1;
      }
    }
  }
  return 0;
}

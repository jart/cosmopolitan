/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/itoa.h"
#include "libc/macros.internal.h"

/**
 * Represents size as readable string.
 *
 * @param p is output buffer
 * @param b should be 1024 or 1000
 * @return pointer to nul byte
 */
char *sizefmt(char *p, uint64_t x, uint64_t b) {
  int i, suffix;
  struct {
    char suffix;
    uint64_t size;
  } kUnits[] = {
      {'e', b * b * b * b * b * b},
      {'p', b * b * b * b * b},
      {'t', b * b * b * b},
      {'g', b * b * b},
      {'m', b * b},
      {'k', b},
  };
  for (suffix = i = 0; i < ARRAYLEN(kUnits); ++i) {
    if (x >= kUnits[i].size * 9) {
      x = (x + kUnits[i].size / 2) / kUnits[i].size;
      suffix = kUnits[i].suffix;
      break;
    }
  }
  p = FormatUint64(p, x);
  if (suffix) *p++ = suffix;
  *p = 0;
  return p;
}

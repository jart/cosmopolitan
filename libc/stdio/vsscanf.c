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
#include "libc/dce.h"
#include "libc/fmt/internal.h"

struct StringScannerState {
  const unsigned char *s;
  size_t i;
};

static int vsscanfcb(void *arg) {
  int res;
  struct StringScannerState *state;
  state = arg;
  if ((res = state->s[state->i])) {
    state->i++;
  } else {
    res = -1;
  }
  return res;
}

/**
 * Decodes string.
 *
 * @see libc/fmt/vcscanf.h (for docs and implementation)
 * @note to offer the best performance, we assume small codebases
 *     needing vsscanf() won't need sscanf() too; and as such, there's
 *     a small code size penalty to using both
 */
int vsscanf(const char *str, const char *fmt, va_list va) {
  struct StringScannerState state = {(const unsigned char *)str, 0};
  return __vcscanf(vsscanfcb, NULL, &state, fmt, va);
}

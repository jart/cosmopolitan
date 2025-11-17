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
#include "libc/intrin/bswap.h"
#include "libc/str/str.h"

struct StringScannerState {
  const char *s;
  char u[8];
  int n;
};

static int vsscanf_getc(void *arg) {
  int c;
  struct StringScannerState *state = arg;
  if (state->n) {
    c = state->u[--state->n] & 255;
  } else if ((c = *state->s & 255)) {
    ++state->s;
  } else {
    c = -1;
  }
  return c;
}

static int vsscanf_ungetc(int c, void *arg) {
  struct StringScannerState *state = arg;
  if (state->n < sizeof(state->u))
    state->u[state->n++] = c;
  return c;
}

static wint_t vsscanf_ungetwc(wint_t c, void *arg) {
  uint64_t w;
  if (!c) {
    vsscanf_ungetc(c, arg);
  } else {
    w = bswap_64(tpenc(c));
    while (!(w & 255))
      w >>= 8;
    do {
      vsscanf_ungetc(w, arg);
    } while ((w >>= 8));
  }
  return c;
}

/**
 * Decodes string.
 */
int vsscanf(const char *str, const char *fmt, va_list va) {
  struct StringScannerState state = {str};
  return __vcscanf(vsscanf_getc,     //
                   vsscanf_ungetc,   //
                   vsscanf_ungetwc,  //
                   &state, fmt, va);
}

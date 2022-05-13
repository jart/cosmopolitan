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
#include "libc/bits/bits.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

static char g_strsignal[12];

/**
 * Returns string describing signal code.
 *
 * This returns SIGZERO for 0 which is the empty value. Textual names
 * should be available for signals 1 through 32. Signals in the range 33
 * and 128 are returned as a `SIG%03d` string. Everything else is SIGWUT
 *
 * @param sig is signal number which should be in range 1 through 128
 * @return pointer to static memory that mutates on subsequent calls
 * @see sigaction()
 */
noasan noinstrument char *strsignal(int sig) {
  char *p;
  const char *s;
  p = g_strsignal;
  p[0] = 'S';
  p[1] = 'I';
  p[2] = 'G';
  p[3] = 0;
  if (sig) {
    if ((s = GetMagnumStr(kSignalNames, sig))) {
      __stpcpy(p + 3, s);
      return p;
    }
  }
  if (!sig) {
    p[3] = 'Z';
    p[4] = 'E';
    p[5] = 'R';
    p[6] = 'O';
    p[7] = 0;
  } else if (1 <= sig && sig <= 128) {
    p[3] = '0' + sig / 100;
    p[4] = '0' + sig / 10 % 10;
    p[5] = '0' + sig % 10;
    p[6] = 0;
  } else {
    p[3] = 'W';
    p[4] = 'U';
    p[5] = 'T';
    p[6] = 0;
  }
  return g_strsignal;
}

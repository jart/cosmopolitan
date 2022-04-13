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
#include "libc/fmt/kerrornames.internal.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

static char g_strsignal[12];

/**
 * Returns string describing signal code.
 *
 * This returns SIGUNKNOWN for 0 which is the empty value. Textual names
 * should be available for signals 1 through 32. Signals in the range 33
 * and 128 are returned as a `SIG%03d` string. Everything else is SIGWUT
 *
 * @param sig is signal number which should be in range 1 through 128
 * @return pointer to static memory that mutates on subsequent calls
 * @see sigaction()
 */
char *strsignal(int sig) {
  int i;
  strcpy(g_strsignal, "SIG");
  if (sig) {
    for (i = 0; kStrSignal[i].x; ++i) {
      if (sig == *(const int *)((uintptr_t)kStrSignal + kStrSignal[i].x)) {
        strcpy(g_strsignal + 3,
               (const char *)((uintptr_t)kStrSignal + kStrSignal[i].s));
        return g_strsignal;
      }
    }
  }
  if (!sig) {
    strcpy(g_strsignal + 3, "UNKNOWN");
  } else if (1 <= sig && sig <= 128) {
    g_strsignal[3] = '0' + sig / 100;
    g_strsignal[4] = '0' + sig / 10 % 10;
    g_strsignal[5] = '0' + sig % 10;
    g_strsignal[6] = 0;
  } else {
    strcpy(g_strsignal + 3, "WUT");
  }
  return g_strsignal;
}
